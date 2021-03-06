#include <iostream>

#include <string>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <new>
#include <factor.h>
#include <Factories.h>


std::string urlDecode(std::string str)
{
    std::string temp;
    int i;
    char tmp[5], tmpchar;
    strncpy(tmp,"0x",5);
    int size = str.size();
    for (i=0; i<size; i++) {
        if (str[i]=='%') {
            if (i+2<size) {
                tmp[2]=str[i+1];
                tmp[3] = str[i+2];
                tmp[4] = '\0';
                tmpchar = (char)strtol(tmp,NULL,0);
                temp+=tmpchar;
                i += 2;
                continue;
            } else {
                break;
            }
        } else if (str[i]=='+') {
            temp+=' ';
        } else {
            temp+=str[i];
        }
    }
    return temp;
}

void initializeGet(std::map <std::string, std::string> &Get)
{
    std::string tmpkey, tmpvalue;
    std::string *tmpstr = &tmpkey;
    char* raw_get = getenv("QUERY_STRING");
    if (raw_get==NULL) {
        Get.clear();
        return;
    }
    while (*raw_get != '\0') {
        if (*raw_get=='&') {
            if (tmpkey!="") {
                Get[urlDecode(tmpkey)] = urlDecode(tmpvalue);
            }
            tmpkey.clear();
            tmpvalue.clear();
            tmpstr = &tmpkey;
        } else if (*raw_get=='=') {
            tmpstr = &tmpvalue;
        } else {
            (*tmpstr) += (*raw_get);
        }
        raw_get++;
    }
    //enter the last pair to the map
    if (tmpkey!="") {
        Get[urlDecode(tmpkey)] = urlDecode(tmpvalue);
        tmpkey.clear();
        tmpvalue.clear();
    }
}

void initializePost(std::map <std::string, std::string> &Post)
{
    std::string tmpkey, tmpvalue;
    std::string *tmpstr = &tmpkey;
    int content_length;
    char *ibuffer;
    char *buffer = NULL;
    char *strlength = getenv("CONTENT_LENGTH");
    if (strlength == NULL) {
        Post.clear();
        return;
    }
    content_length = atoi(strlength);
    if (content_length == 0) {
        Post.clear();
        return;
    }

    try {
        buffer = new char[content_length*sizeof(char)];
    } catch (std::bad_alloc xa) {
        Post.clear();
        return;
    }
    if(fread(buffer, sizeof(char), content_length, stdin) != (unsigned int)content_length) {
        Post.clear();
        return;
    }
    *(buffer+content_length) = '\0';
    ibuffer = buffer;
    while (*ibuffer != '\0') {
        if (*ibuffer=='&') {
            if (tmpkey!="") {
                Post[urlDecode(tmpkey)] = urlDecode(tmpvalue);
            }
            tmpkey.clear();
            tmpvalue.clear();
            tmpstr = &tmpkey;
        } else if (*ibuffer=='=') {
            tmpstr = &tmpvalue;
        } else {
            (*tmpstr) += (*ibuffer);
        }
        ibuffer++;
    }
    //enter the last pair to the map
    if (tmpkey!="") {
        Post[urlDecode(tmpkey)] = urlDecode(tmpvalue);
        tmpkey.clear();
        tmpvalue.clear();
    }
}




int main() {

    std::map <std::string, std::string> Post;
    initializePost(Post) ;

    if(Post.count("req"))
    {
        std::string sRes = bayeslib::SessionEntry::RunCommand(Post["req"]);
        std::cout << "Content-Type:text/html:" << std::endl << std::endl;
        std::cout << sRes.c_str() << std::endl;
    }
    else
    {
        std::cout << "Content-Type:text/html:" << std::endl << std::endl;
        std::cout << "{\"res\":\"failed\"}" << std::endl;

    }

    return 0;
}
