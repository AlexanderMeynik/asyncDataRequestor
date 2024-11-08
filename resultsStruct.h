
#ifndef POSTGESSTEST_RESULTSSTRUCT_H
#define POSTGESSTEST_RESULTSSTRUCT_H
#include <indicators/progress_bar.hpp>
#include <functional>
#include <algorithm>
#include <string>
#include <pugixml.hpp>
#include <hash_map>
#include <map>
#include <iostream>
#include "databaseService.h"
#include <pqxx/pqxx>
#include <quill/Logger.h>
#include <quill/detail/LogMacros.h>
using namespace pugi;
constexpr std::array<int,3>longints={3,4,10/*,11,12*/};//check for parse ll
constexpr const char* default_update="UPDATE articles SET ";


void static strip(std::string& str);


const std::map<std::string,int>fname_to_index={
        {"url",0},
        {"title",1},
        //{"issn",2},
        {"starting_page",3},
        {"ending_page",4},
        {"cover_date",5},
        {"creators",7},
        {"description",6},
        {"subjects",8},
        {"scopus_id",9},
        {"volume",10},
        {"issue_identifier",11},
        {"number",12}
};


//std::function<std::string(xml_document,std::string)>arr

  //      [](xml_document&doc){return "strung";}};
class resultsStruct {
public:
    std::string getIssn();
    void parseStruct(std::string &body, quill::Logger *pLogger);
    std::string constructUpdateQuery(quill::Logger *pLogger);

private:



    /*std::map<std::string,int>fname_to_id={

    };*/

    std::array<std::string,13> fields;


   /* std::string url;
    std::string title;
    std::string issn;
    std::string starting_page;
    std::string ending_page;
    std::string publication_date;
    std::string description;*/
    //std::vector<std::string> authors;
    //std::vector<std::string> subjects;
    //long long int scopus_id;
    //long long int volume;
    //long long int issue_identifier;
    //long long int number;

    pqxx::connection didcatedConnection=pqxx::connection(
            "dbname=sciencedirect user=scuser password=rar4Muga \
      hostaddr=127.0.0.1 port=5432");
    const std::array<std::function<std::string(pugi::xml_node&)>,13> functions={
            [](pugi::xml_node&root){return root.child("coredata").child_value("prism:url");},
            [](pugi::xml_node&root){return root.child("coredata").child_value("dc:title");},
            [](pugi::xml_node&root){return root.child("coredata").child_value("prism:issn");},
            [](pugi::xml_node&root){return root.child("coredata").child_value("prism:startingPage");},
            [](pugi::xml_node&root){return root.child("coredata").child_value("prism:endingPage");},
            [](pugi::xml_node&root){return root.child("coredata").child_value("prism:coverDate");},
            [](pugi::xml_node&root){return root.child("coredata").child_value("dc:description");},
            [this](pugi::xml_node&root){
                std::string creators = "{";


                auto txn=pqxx::work(didcatedConnection);
                for (pugi::xml_node creator : root.child("coredata").children("dc:creator"))
                {
                    creators +=txn.quote(pqxx::to_string(std::string(creator.text().get()))) + ",";
                }
                if (!creators.empty() && creators.back() == ',')
                {
                    creators.pop_back(); // Remove trailing comma
                }
                creators += "}";
                txn.abort();
                return creators;},

            [this](pugi::xml_node&root){
                std::string subjects = "{";
                for (pugi::xml_node subject : root.child("coredata").children("dcterms:subject"))
                {

                    std::string subjectText = subject.text().get();

                    auto start_it = subjectText.begin();
                    auto end_it = subjectText.rbegin();
                    while (std::isspace(*start_it)) {
                        ++start_it;
                        if (start_it == subjectText.end()) break;
                    }
                    while (std::isspace(*end_it)) {
                        ++end_it;
                        if (end_it == subjectText.rend()) break;
                    }
                    int start_pos = start_it - subjectText.begin();
                    int end_pos = end_it.base() - subjectText.begin();
                    subjectText = start_pos <= end_pos ? std::string(start_it, end_it.base()) : "";


                    //strip(subjectText);

                    /*// Find the position of the newline character and truncate the string
                    size_t newlinePos = subjectText.find('\n');
                    if (newlinePos != std::string::npos)
                    {
                        subjectText = subjectText.substr(0, newlinePos);
                    }*/
                    auto txn=pqxx::work(didcatedConnection);

                    subjects += txn.quote(pqxx::to_string(subjectText))+",";
                    txn.abort();
                }
                if (!subjects.empty() && subjects.back() == ',')
                {
                    subjects.pop_back(); // Remove trailing comma
                }
                subjects += "}";
                return subjects;
            },
            [](pugi::xml_node&root){return root.child("scopus-id").text().get();},
            [](pugi::xml_node&root){return root.child("coredata").child("prism:volume").text().get();;},
            [](pugi::xml_node&root){return root.child("coredata").child("prism:issueIdentifier").text().get();},
            [](pugi::xml_node&root){return root.child("coredata").child("prism:number").text().get();}
    };

   // static auto const constexpr func=[](xml_document&doc,std::string&query){return "strung";};
//std::vector<std::function<std::string(xml_document&)>> arr={
};


#endif //POSTGESSTEST_RESULTSSTRUCT_H
