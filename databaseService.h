
#ifndef POSTGESSTEST_DATABASESERVICE_H
#define POSTGESSTEST_DATABASESERVICE_H
#include <pqxx/pqxx>

#include <quill/Logger.h>
#include <quill/detail/LogMacros.h>

class databaseService {

public:
    pqxx::work getTransaction();
    void updateArticles(std::string& query,std::string&id,quill::Logger *pLogger);
    void journalISSN(int &id, std::string &issn, quill::Logger *pLogger);


    void articleIsmissing(std::string&id,quill::Logger *pLogger);

    std::vector<std::string> get_article_links(int id,quill::Logger *pLogger);
    std::vector<int> get_journal_links2(quill::Logger *pLogger);

    ~databaseService();

private:
    pqxx::connection conn=pqxx::connection(
            "dbname=sciencedirect user=scuser password=rar4Muga \
      hostaddr=127.0.0.1 port=5432");
};


#endif //POSTGESSTEST_DATABASESERVICE_H
