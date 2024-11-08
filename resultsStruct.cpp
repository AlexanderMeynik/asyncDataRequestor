//
// Created by Lenovo on 07.11.2023.
//



#include "resultsStruct.h"

void resultsStruct::parseStruct(std::string &body, quill::Logger *pLogger) {
    xml_document doc;
    xml_parse_result res=doc.load_string(body.c_str());

    pugi::xml_node root = doc.child("full-text-retrieval-response");
    for (int i = 0; i < fields.size(); ++i) {
        fields[i]=functions[i](root);
    }
    for (auto & field : fields) {
        if(field.empty()||field=="{}")
        {
            field="NULL";
        }
    }
    for(auto idx:longints)
    {
        try {
            long long lonint = std::stoi(fields[idx]);

        }
        catch (...)
        {
            fields[idx]="NULL";
        }
    }


    try {
        long long lonint = std::stoll(fields[9]);
        //fields[9]=std::to_string(lonint);
    }
    catch (...)
    {
        fields[9]="NULL";
    }


}

std::string resultsStruct::constructUpdateQuery(quill::Logger *pLogger) {


    std::ostringstream sql;
    sql<<default_update;

    bool firstField = true;

    auto txn=pqxx::transaction(this->didcatedConnection);
    for (const auto& [field, index] : fname_to_index)
    {
        if (fields[index] != "NULL")
        {
            if (!firstField)
            {
                sql << ", ";
            }
            sql << field << " = " << txn.quote(fields[index]);


            firstField = false;
        }
        txn.abort();
    }
    std::string stst=sql.str();
    if(stst==default_update)
    {
        stst+="status = 3";
        LOG_INFO(pLogger,"Missing article");
    }else
    { stst+=", status = 2";}
    return stst;
}

std::string resultsStruct::getIssn() {
    return fields[2];
}

void static strip(std::string &str) {
        if (str.length() == 0) {
            return;
        }

        auto start_it = str.begin();
        auto end_it = str.rbegin();
        while (std::isspace(*start_it)) {
            ++start_it;
            if (start_it == str.end()) break;
        }
        while (std::isspace(*end_it)) {
            ++end_it;
            if (end_it == str.rend()) break;
        }
        int start_pos = start_it - str.begin();
        int end_pos = end_it.base() - str.begin();
        str = start_pos <= end_pos ? std::string(start_it, end_it.base()) : "";
}
