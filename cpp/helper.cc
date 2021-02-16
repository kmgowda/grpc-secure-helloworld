/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "helper.h"
#include <string.h>
#include <fstream>


bool readFile(const std::string &filepath,std::string &buffer){
    std::ifstream fin(filepath.c_str());
    if (fin.good()) {
        std::cout<< "file :"+filepath+" found!" <<std::endl;
        getline(fin, buffer, (char) fin.eof());
        fin.close();
        return true;
    }
    std::cout<< "file :"+filepath+" not found!" <<std::endl;
    return false;
}

void readCertParams(int argc, char** argv, SecureParams &out) {
    for (int i=0; i < argc ; i++) {
        if (strcasecmp(argv[i],"-ca")==0) {
            out.caFile = argv[i+1];
        } else if (strcasecmp(argv[i],"-cert")==0) {
            out.certFile =  argv[i+1];
        } else if (strcasecmp(argv[i],"-key")==0) {
            out.keyFile =  argv[i+1];
        }
    }
}

bool parseSecureCredentials(SecureParams &params, SecureCredentials &out) {
    if (params.caFile.empty()) {
        out.ca = "";
    } else {
        readFile(params.caFile, out.ca);
    }
    if (params.certFile.empty()) {
        std::cout << "Supply certificate file with option -cert " << std::endl;
        return false;
    }
    if (params.keyFile.empty()) {
        std::cout << "Supply key file with option -key " << std::endl;
        return false;
    }
    if (!readFile(params.certFile, out.cert))   {
        return false;
    }
    return readFile(params.keyFile, out.key);
}