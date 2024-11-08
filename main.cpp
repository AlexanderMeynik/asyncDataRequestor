
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include "databaseService.h"

#include <cpr/cpr.h>
#include <quill/Quill.h>
#include <indicators/block_progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <omp.h>
#include "resultsStruct.h"
const int numthreads=16;
cpr::Response getRes(std::string&id,std::string apiKey)
{

    cpr::Response r = cpr::Get(cpr::Url{"https://api.elsevier.com/content/article/pii/"+id},
                               cpr::Parameters{{"META_ABS","view"}},
                               cpr::Header{{"X-ELS-APIKey", apiKey}});
    return r;

}

std::string api[numthreads];

void readKeys()
{
    std::ifstream in("../keys.txt");
    //todo get your elsiver keys here https://dev.elsevier.com/index.jsp
    for (int i = 0; i < numthreads; ++i) {
        in>>api[i];
        if(!in)
        {
            abort();
        }
    }
    in.close();
}


int main(int argc, char** argv) {





    quill::configure(
            []()
            {
                quill::Config cfg;
                return cfg;
            }());

    quill::start();



    quill::Logger* logger = quill::create_logger(
            "file_logger",
            quill::file_handler("main.log",
                                []()
                                {
                                    quill::FileHandlerConfig cfg;
                                    cfg.set_open_mode('w');
                                    cfg.set_pattern(
                                            "[%(ascii_time)] [%(thread)] [%(filename):%(lineno)] [%(logger_name)] "
                                            "[%(level_name)] - %(message)",
                                            "%H:%M:%S.%Qms");
                                    return cfg;
                                }()));

    logger->set_log_level(quill::LogLevel::TraceL3);
    readKeys();

    LOG_INFO(logger, "Keys retrieved successfully!");


    databaseService db;


    std::vector<databaseService>dbs= std::vector<databaseService>(numthreads);

    auto journals=db.get_journal_links2(logger);
    journals.erase(journals.begin(),journals.begin());



    using namespace indicators;
    BlockProgressBar bar{
            option::BarWidth{80},
            option::ForegroundColor{Color::white},
            option::FontStyles{
                    std::vector<FontStyle>{FontStyle::bold}},
            option::MaxProgress{journals.size()},
            option::ShowElapsedTime{true},
            option::ShowRemainingTime{true},

            option::FontStyles{std::vector<FontStyle>{FontStyle::bold}},
    };


    omp_set_num_threads(numthreads);
    #pragma omp parallel for shared(journals,api,dbs,logger,bar) default(none)
    for(int i=0;i<journals.size();i++)
    {
        int journal_id=journals[i];
        int thread_id=omp_get_thread_num();



        std::vector<std::string> articles=dbs[thread_id].get_article_links(journal_id,logger);
        std::string issn;

        std::vector<cpr::AsyncResponse> container;


        for(int j=0;j<articles.size();j++) {
            std::string a_id = articles[j];
            container.emplace_back(cpr::GetAsync(cpr::Url{"https://api.elsevier.com/content/article/pii/"+a_id},
                                                 cpr::Parameters{{"META_ABS","view"}},
                                                 cpr::ReserveSize{1024 * 100},
                                                 cpr::Header{{"X-ELS-APIKey", api[thread_id]}}));
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }

        LOG_INFO(logger, "Quereing info done!");



        for(int j=0;j<articles.size();j++) {
            std::string a_id = articles[j];
            cpr::Response resp = container[j].get();

            if (resp.status_code != 200) {
                if (resp.header["x-els-status"] == "RESOURCE_NOT_FOUND - The resource specified cannot be found.") {
                    dbs[thread_id].articleIsmissing(a_id,logger);
                } else {
                    LOG_ERROR(logger, "Failed to retrieve data.Id = {}. Status code {},Status str {}!",a_id,resp.status_code,resp.header["x-els-status"]);
                }
            } else {
                std::string body = resp.text;
                resultsStruct rs;
                rs.parseStruct(body, logger);
                std::string query = rs.constructUpdateQuery(logger);

                dbs[thread_id].updateArticles(query, a_id, logger);
                issn = rs.getIssn();

            }

        }



        if (issn != "NULL") {
            LOG_INFO(logger, "Journal {} set issn {}+ articlenum={}!", journal_id, issn,articles.size());
            dbs[thread_id].journalISSN(journal_id, issn, logger);

        }
        bar.tick();
    }


    bar.mark_as_completed();

    indicators::show_console_cursor(true);




    return 0;
}