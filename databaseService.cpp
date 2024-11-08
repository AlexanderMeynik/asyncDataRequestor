

#include <iostream>

#include "databaseService.h"


pqxx::work databaseService::getTransaction() {
    return pqxx::work(conn);
}

databaseService::~databaseService() {

}

void databaseService::updateArticles(std::string &query,std::string&id,quill::Logger *pLogger) {
    try
    {
    auto txn= this->getTransaction();
    query+=" WHERE article_id = "+txn.quote(id)+";";
    txn.exec(query);
        txn.commit();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(pLogger, "DbError!{}. articleid={}",e.what(),id);
    }



}

void databaseService::journalISSN(int &id, std::string &issn, quill::Logger *pLogger) {
    try
    {

        std::string query = "UPDATE journals SET issn = $1 WHERE id = $2";

        auto txn = this->getTransaction();


        pqxx::result r3=txn.exec_params(query,issn,id);
        txn.commit();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(pLogger, "Set issn error.DbError!{}. Journal {}, Issn {}",e.what(),id,issn);

    }
}


std::vector<int> databaseService::get_journal_links2(quill::Logger *pLogger) {
    std::vector<int> rows;
    try {
        pqxx::work txn = this->getTransaction();

        pqxx::result result = txn.exec("SELECT DISTINCT j.id AS journal_id FROM journals j JOIN articles a ON j.id = a.journal_id WHERE a.status = 1 order by j.id;");

        for (const auto& row : result) {
            rows.push_back(row["journal_id"].as<int>());
        }

        txn.commit();

    } catch (const std::exception& e) {

        LOG_ERROR(pLogger, "Ошибка при заборе имён журналов DbError!{}",e.what());
        exit(1);
    }

    return rows;
}

std::vector<std::string> databaseService::get_article_links(int id,quill::Logger *pLogger) {
    std::vector<std::string> rows;
    try {
        pqxx::work txn = this->getTransaction();

        pqxx::result result = txn.exec_params("SELECT article_id, journal_id FROM articles WHERE journal_id = $1 AND status = 1;", id);

        for (const auto& row : result) {
            rows.push_back(row["article_id"].as<std::string>());
        }

        txn.commit();

    } catch (const std::exception& e) {

        LOG_ERROR(pLogger, "Ошибка при заборе имён статей: DbError!{}, j_id ={}",e.what(),id);
        exit(1);
    }

    return rows;
}

void databaseService::articleIsmissing(std::string &id,quill::Logger *pLogger) {
    try
    {

        std::string query = "UPDATE articles SET status = 3 WHERE article_id = $1";

        auto txn = this->getTransaction();

        pqxx::result r3=txn.exec_params(query,id);
        txn.commit();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(pLogger, "Set missing error.DbError!{}. Journal_id = {}",e.what(),id);
        std::cerr << "Error: " << e.what() << std::endl;//todo log
    }
}

