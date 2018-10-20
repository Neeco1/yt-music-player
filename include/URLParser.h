#ifndef URL_PARSER_H__
#define URL_PARSER_H__

#include <string>
#include <regex>

class URLParser {
public:
    static bool parseURL_YoutubePlaylist(std::string url, std::string & listId) {
        //Youtube Playlist URL regex
        std::regex yt_plist_regex("^.*(youtu.be\\/|list=)([^#\\&\\?]*).*");
        
        std::smatch matches;
        if(std::regex_match(url, matches, yt_plist_regex))
        {
            //Return id value by parameter
            listId = matches[2].str();
            return true;
        }
        return false;
    }

};

#endif
