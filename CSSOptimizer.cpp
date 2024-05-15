#include "CSSOptimizer.h"

#include <string>
#include <map>
#include <set>
#include <cctype>
#include <algorithm>
#include <charconv>
#include <cstring>

using StringViewMap = std::map<std::string_view,std::string_view>;
using PropertiesMap = std::map<std::string,std::set<std::string>>;

static constexpr std::initializer_list<std::pair<std::string_view, std::string_view>> nameToHexList = {

    {"indianred", "#cd5c5c"},
    {"lightcoral", "#f08080"},
    {"salmon", "#fa8072"},
    {"darksalmon", "#e9967a"},
    {"lightsalmon", "#ffa07a"},
    {"crimson", "#dc143c"},
    {"red", "#f00"},
    {"firebrick", "#b22222"},
    {"darkred", "#8b0000"},
    {"pink", "#ffc0cb"},
    {"lightpink", "#ffb6c1"},
    {"hotpink", "#ff69b4"},
    {"deeppink", "#ff1493"},
    {"mediumvioletred", "#c71585"},
    {"palevioletred", "#db7093"},
    {"lightsalmon", "#ffa07a"},
    {"coral", "#ff7f50"},
    {"tomato", "#ff6347"},
    {"orangered", "#ff4500"},
    {"darkorange", "#ff8c00"},
    {"orange", "#ffa500"},
    {"gold", "#ffd700"},
    {"yellow", "#ff0"},
    {"lightyellow", "#ffffe0"},
    {"lemonchiffon", "#fffacd"},
    {"lightgoldenrodyellow", "#fafad2"},
    {"papayawhip", "#ffefd5"},
    {"moccasin", "#ffe4b5"},
    {"peachpuff", "#ffdab9"},
    {"palegoldenrod", "#eee8aa"},
    {"khaki", "#f0e68c"},
    {"darkkhaki", "#bdb76b"},
    {"lavender", "#e6e6fa"},
    {"thistle", "#d8bfd8"},
    {"plum", "#dda0dd"},
    {"violet", "#ee82ee"},
    {"orchid", "#da70d6"},
    {"fuchsia", "#f0f"},
    {"magenta", "#f0f"},
    {"mediumorchid", "#ba55d3"},
    {"mediumpurple", "#9370db"},
    {"rebeccapurple", "#639"},
    {"blueviolet", "#8a2be2"},
    {"darkviolet", "#9400d3"},
    {"darkorchid", "#9932cc"},
    {"darkmagenta", "#8b008b"},
    {"purple", "#800080"},
    {"indigo", "#4b0082"},
    {"slateblue", "#6a5acd"},
    {"darkslateblue", "#483d8b"},
    {"mediumslateblue", "#7b68ee"},
    {"greenyellow", "#adff2f"},
    {"chartreuse", "#7fff00"},
    {"lawngreen", "#7cfc00"},
    {"lime", "#0f0"},
    {"limegreen", "#32cd32"},
    {"palegreen", "#98fb98"},
    {"lightgreen", "#90ee90"},
    {"mediumspringgreen", "#00fa9a"},
    {"springgreen", "#00ff7f"},
    {"mediumseagreen", "#3cb371"},
    {"seagreen", "#2e8b57"},
    {"forestgreen", "#228b22"},
    {"green", "#008000"},
    {"darkgreen", "#006400"},
    {"yellowgreen", "#9acd32"},
    {"olivedrab", "#6b8e23"},
    {"olive", "#808000"},
    {"darkolivegreen", "#556b2f"},
    {"mediumaquamarine", "#66cdaa"},
    {"darkseagreen", "#8fbc8b"},
    {"lightseagreen", "#20b2aa"},
    {"darkcyan", "#008b8b"},
    {"teal", "#008080"},
    {"aqua", "#0ff"},
    {"cyan", "#0ff"},
    {"lightcyan", "#e0ffff"},
    {"paleturquoise", "#afeeee"},
    {"aquamarine", "#7fffd4"},
    {"turquoise", "#40e0d0"},
    {"mediumturquoise", "#48d1cc"},
    {"darkturquoise", "#00ced1"},
    {"cadetblue", "#5f9ea0"},
    {"steelblue", "#4682b4"},
    {"lightsteelblue", "#b0c4de"},
    {"powderblue", "#b0e0e6"},
    {"lightblue", "#add8e6"},
    {"skyblue", "#87ceeb"},
    {"lightskyblue", "#87cefa"},
    {"deepskyblue", "#00bfff"},
    {"dodgerblue", "#1e90ff"},
    {"cornflowerblue", "#6495ed"},
    {"mediumslateblue", "#7b68ee"},
    {"royalblue", "#4169e1"},
    {"blue", "#00f"},
    {"mediumblue", "#0000cd"},
    {"darkblue", "#00008b"},
    {"navy", "#000080"},
    {"midnightblue", "#191970"},
    {"cornsilk", "#fff8dc"},
    {"blanchedalmond", "#ffebcd"},
    {"bisque", "#ffe4c4"},
    {"navajowhite", "#ffdead"},
    {"wheat", "#f5deb3"},
    {"burlywood", "#deb887"},
    {"tan", "#d2b48c"},
    {"rosybrown", "#bc8f8f"},
    {"sandybrown", "#f4a460"},
    {"goldenrod", "#daa520"},
    {"darkgoldenrod", "#b8860b"},
    {"peru", "#cd853f"},
    {"chocolate", "#d2691e"},
    {"saddlebrown", "#8b4513"},
    {"sienna", "#a0522d"},
    {"brown", "#a52a2a"},
    {"maroon", "#800000"},
    {"white", "#fff"},
    {"snow", "#fffafa"},
    {"honeydew", "#f0fff0"},
    {"mintcream", "#f5fffa"},
    {"azure", "#f0ffff"},
    {"aliceblue", "#f0f8ff"},
    {"ghostwhite", "#f8f8ff"},
    {"whitesmoke", "#f5f5f5"},
    {"seashell", "#fff5ee"},
    {"beige", "#f5f5dc"},
    {"oldlace", "#fdf5e6"},
    {"floralwhite", "#fffaf0"},
    {"ivory", "#fffff0"},
    {"antiquewhite", "#faebd7"},
    {"linen", "#faf0e6"},
    {"lavenderblush", "#fff0f5"},
    {"mistyrose", "#ffe4e1"},
    {"gainsboro", "#dcdcdc"},
    {"lightgray", "#d3d3d3"},
    {"silver", "#c0c0c0"},
    {"darkgray", "#a9a9a9"},
    {"gray", "#808080"},
    {"dimgray", "#696969"},
    {"lightslategray", "#789"},
    {"slategray", "#708090"},
    {"darkslategray", "#2f4f4f"},
    {"black", "#000"}

};

static const StringViewMap nameToHex(nameToHexList.begin(),nameToHexList.end());

static const StringViewMap hexToName = {

    {"#cd5c5c", "indianred"},
    {"#f08080", "lightcoral"},
    {"#fa8072", "salmon"},
    {"#e9967a", "darksalmon"},
    {"#ffa07a", "lightsalmon"},
    {"#dc143c", "crimson"},
    {"#f00", "red"},
    {"#b22222", "firebrick"},
    {"#8b0000", "darkred"},
    {"#ffc0cb", "pink"},
    {"#ffb6c1", "lightpink"},
    {"#ff69b4", "hotpink"},
    {"#ff1493", "deeppink"},
    {"#c71585", "mediumvioletred"},
    {"#db7093", "palevioletred"},
    {"#ffa07a", "lightsalmon"},
    {"#ff7f50", "coral"},
    {"#ff6347", "tomato"},
    {"#ff4500", "orangered"},
    {"#ff8c00", "darkorange"},
    {"#ffa500", "orange"},
    {"#ffd700", "gold"},
    {"#ff0", "yellow"},
    {"#ffffe0", "lightyellow"},
    {"#fffacd", "lemonchiffon"},
    {"#fafad2", "lightgoldenrodyellow"},
    {"#ffefd5", "papayawhip"},
    {"#ffe4b5", "moccasin"},
    {"#ffdab9", "peachpuff"},
    {"#eee8aa", "palegoldenrod"},
    {"#f0e68c", "khaki"},
    {"#bdb76b", "darkkhaki"},
    {"#e6e6fa", "lavender"},
    {"#d8bfd8", "thistle"},
    {"#dda0dd", "plum"},
    {"#ee82ee", "violet"},
    {"#da70d6", "orchid"},
    {"#f0f", "fuchsia"},
    {"#f0f", "magenta"},
    {"#ba55d3", "mediumorchid"},
    {"#9370db", "mediumpurple"},
    {"#639", "rebeccapurple"},
    {"#8a2be2", "blueviolet"},
    {"#9400d3", "darkviolet"},
    {"#9932cc", "darkorchid"},
    {"#8b008b", "darkmagenta"},
    {"#800080", "purple"},
    {"#4b0082", "indigo"},
    {"#6a5acd", "slateblue"},
    {"#483d8b", "darkslateblue"},
    {"#7b68ee", "mediumslateblue"},
    {"#adff2f", "greenyellow"},
    {"#7fff00", "chartreuse"},
    {"#7cfc00", "lawngreen"},
    {"#0f0", "lime"},
    {"#32cd32", "limegreen"},
    {"#98fb98", "palegreen"},
    {"#90ee90", "lightgreen"},
    {"#00fa9a", "mediumspringgreen"},
    {"#00ff7f", "springgreen"},
    {"#3cb371", "mediumseagreen"},
    {"#2e8b57", "seagreen"},
    {"#228b22", "forestgreen"},
    {"#008000", "green"},
    {"#006400", "darkgreen"},
    {"#9acd32", "yellowgreen"},
    {"#6b8e23", "olivedrab"},
    {"#808000", "olive"},
    {"#556b2f", "darkolivegreen"},
    {"#66cdaa", "mediumaquamarine"},
    {"#8fbc8b", "darkseagreen"},
    {"#20b2aa", "lightseagreen"},
    {"#008b8b", "darkcyan"},
    {"#008080", "teal"},
    {"#0ff", "aqua"},
    {"#0ff", "cyan"},
    {"#e0ffff", "lightcyan"},
    {"#afeeee", "paleturquoise"},
    {"#7fffd4", "aquamarine"},
    {"#40e0d0", "turquoise"},
    {"#48d1cc", "mediumturquoise"},
    {"#00ced1", "darkturquoise"},
    {"#5f9ea0", "cadetblue"},
    {"#4682b4", "steelblue"},
    {"#b0c4de", "lightsteelblue"},
    {"#b0e0e6", "powderblue"},
    {"#add8e6", "lightblue"},
    {"#87ceeb", "skyblue"},
    {"#87cefa", "lightskyblue"},
    {"#00bfff", "deepskyblue"},
    {"#1e90ff", "dodgerblue"},
    {"#6495ed", "cornflowerblue"},
    {"#7b68ee", "mediumslateblue"},
    {"#4169e1", "royalblue"},
    {"#00f", "blue"},
    {"#0000cd", "mediumblue"},
    {"#00008b", "darkblue"},
    {"#000080", "navy"},
    {"#191970", "midnightblue"},
    {"#fff8dc", "cornsilk"},
    {"#ffebcd", "blanchedalmond"},
    {"#ffe4c4", "bisque"},
    {"#ffdead", "navajowhite"},
    {"#f5deb3", "wheat"},
    {"#deb887", "burlywood"},
    {"#d2b48c", "tan"},
    {"#bc8f8f", "rosybrown"},
    {"#f4a460", "sandybrown"},
    {"#daa520", "goldenrod"},
    {"#b8860b", "darkgoldenrod"},
    {"#cd853f", "peru"},
    {"#d2691e", "chocolate"},
    {"#8b4513", "saddlebrown"},
    {"#a0522d", "sienna"},
    {"#a52a2a", "brown"},
    {"#800000", "maroon"},
    {"#fff", "white"},
    {"#fffafa", "snow"},
    {"#f0fff0", "honeydew"},
    {"#f5fffa", "mintcream"},
    {"#f0ffff", "azure"},
    {"#f0f8ff", "aliceblue"},
    {"#f8f8ff", "ghostwhite"},
    {"#f5f5f5", "whitesmoke"},
    {"#fff5ee", "seashell"},
    {"#f5f5dc", "beige"},
    {"#fdf5e6", "oldlace"},
    {"#fffaf0", "floralwhite"},
    {"#fffff0", "ivory"},
    {"#faebd7", "antiquewhite"},
    {"#faf0e6", "linen"},
    {"#fff0f5", "lavenderblush"},
    {"#ffe4e1", "mistyrose"},
    {"#dcdcdc", "gainsboro"},
    {"#d3d3d3", "lightgray"},
    {"#c0c0c0", "silver"},
    {"#a9a9a9", "darkgray"},
    {"#808080", "gray"},
    {"#696969", "dimgray"},
    {"#789", "lightslategray"},
    {"#708090", "slategray"},
    {"#2f4f4f", "darkslategray"},
    {"#000", "black"}

};

class StreamTask
{
protected:
   size_t pos = 0;
public:    
   virtual ~StreamTask(){}

   virtual void addSeparator(char c) = 0;
   virtual bool addWord(std::string_view word) = 0;
   virtual bool finish(std::string & str) = 0;

   void setPos(size_t pos){ this->pos = pos; }
};

static bool shortHexRGB(std::string_view color)
{
   if(color[1] == color[2] && color[3] == color[4] && color[5] == color[6])
   {
      const_cast<char *>(color.data())[2] = const_cast<char *>(color.data())[3];
      const_cast<char *>(color.data())[3] = const_cast<char *>(color.data())[5];

      return true;
   }

   return false;
}

static bool shortHexRGBA(std::string_view color)
{
   if(color[1] == color[2] && color[3] == color[4] && color[5] == color[6] && color[7] == color[8])
   {
      const_cast<char *>(color.data())[2] = const_cast<char *>(color.data())[3];
      const_cast<char *>(color.data())[3] = const_cast<char *>(color.data())[5];
      const_cast<char *>(color.data())[4] = const_cast<char *>(color.data())[7];

      return true;
   }

   return false;
}

static std::string_view getNameColor(std::string_view hex)
{
   if(auto find = hexToName.find(hex); find != hexToName.end()) return find->second;

   return std::string_view();
}

static std::string_view getHexColor(std::string_view name)
{
   if(auto find = nameToHex.find(name); find != nameToHex.end()) return find->second;

   return std::string_view();
}

static void dataReplace(std::string_view before, std::string_view after)
{
   std::strncpy(const_cast<char *>(before.data()), const_cast<char *>(after.data()), after.length());
}

static const char * const hex = "0123456789abcdef";

class RGBTask final : public StreamTask
{
   bool valid = true;
   uint8_t data[4] = {};

public:
   void addSeparator(char c) override
   {
      if(data[0] == 0){ if(c == '(') return; }
      else if(data[0] == 1){ if(c == ',') return; }
      else if(data[0] == 2){ if(c == ',') return; }
      else if(data[0] == 3)
      {
         if(c == ')')
         {
            data[0] = data[0] + 1;
            return;
         }
      }

      valid = false;
   }

   bool addWord(std::string_view word) override
   {
      if(!valid) return false;

      data[0] = data[0] + 1;

      auto [ptr, ec] = std::from_chars(word.data(),word.data()+word.length(), data[data[0]]);

      if (ec != std::errc()) return false;

      return true;
   }

   bool finish(std::string & str) override
   {
      if(!valid || data[0] != 4) return false;

      str[pos] = '#';

      for(int i = 1; i <= 3; i++)
      {
         pos++;
         str[pos] = hex[((data[i]&0xf0)>>4)];
         pos++;
         str[pos] = hex[(data[i]&0x0f)];
      }

      auto hex = std::string_view(str.data() + pos - 6, 7);

      if(shortHexRGB(hex)) hex = hex.substr(0,4);

      auto name = getNameColor(hex);

      if(name.length() > 0 && hex.length() > name.length())
      {
         dataReplace(hex, name);
         hex = hex.substr(0, name.length());
      }

      str.resize(hex.data()-str.data()+hex.length());

      return false;
   }
};

class RGBATask final : public StreamTask
{
    bool valid = true;
    uint8_t data[5] = {};

public:
    void addSeparator(char c) override
    {
        if(data[0] == 0){ if(c == '(') return; }
        else if(data[0] == 1){ if(c == ',') return; }
        else if(data[0] == 2){ if(c == ',') return; }
        else if(data[0] == 3){ if(c == ',') return; }
        else if(data[0] == 4)
        {
            if(c == ')')
            {
                data[0] = data[0] + 1;
                return;
            }
        }

        valid = false;
    }

    bool addWord(std::string_view word) override
    {
        if(!valid) return false;

        data[0] = data[0] + 1;

        auto [ptr, ec] = std::from_chars(word.data(),word.data()+word.length(), data[data[0]]);

        if (ec != std::errc()) return false;

        return true;
    }

    bool finish(std::string & str) override
    {
        if(!valid || data[0] != 5) return false;

        str[pos] = '#';

        for(int i = 1; i <= 4; i++)
        {
            pos++;
            str[pos] = hex[((data[i]&0xf0)>>4)];
            pos++;
            str[pos] = hex[(data[i]&0x0f)];
        }

        auto hex = std::string_view(str.data() + pos - 8, 9);

        if(hex.ends_with("ff"))
        {
           hex = hex.substr(0,7);

           if(shortHexRGB(hex)) hex = hex.substr(0,4);

            auto name = getNameColor(hex);

            if(name.length() > 0 && hex.length() > name.length())
            {
                dataReplace(hex, name);
                hex = hex.substr(0, name.length());
            }
        }
        else if(shortHexRGBA(hex)) hex = hex.substr(0,5);

        str.resize(hex.data()-str.data()+hex.length());

        return false;
    }
};

static constexpr size_t maxTaskSize()
{
   const std::initializer_list<size_t> tasks =
   {
      sizeof(RGBTask),
      sizeof(RGBATask)
   };

   return *std::max_element(tasks.begin(), tasks.end(), [](auto a, auto b) constexpr -> bool { return a < b; });
}

static constexpr size_t maxListSize(std::initializer_list<std::pair<std::string_view, std::string_view>> list)
{
   return std::max_element(list.begin(), list.end(), [](auto a, auto b) constexpr -> bool { return a.first.size() < b.first.size(); })->first.size();
}

static constexpr size_t minListSize(std::initializer_list<std::pair<std::string_view, std::string_view>> list)
{
   return std::min_element(list.begin(), list.end(), [](auto a, auto b) constexpr -> bool { return a.first.size() < b.first.size(); })->first.size();
}

static std::pair<size_t,StreamTask*> findColor(std::string_view word, char * heap)
{
   static constexpr size_t minKeyNameToHex = minListSize(nameToHexList), maxKeyNameToHex = maxListSize(nameToHexList);

   if(word[0] == '#')
   {
      if(word.length() == 4)
      {
         auto name = getNameColor(word);

         if(name.length() > 0 && word.length() > name.length())
         {
            dataReplace(word, name);
            return {name.length(), nullptr};
         }
      }
      else if(word.length() == 7)
      {
         if(shortHexRGB(word)) word = word.substr(0,4);

         auto name = getNameColor(word);

         if(name.length() > 0 && word.length() > name.length())
         {
            dataReplace(word, name);
            return {name.length(),nullptr};
         }
      }
      else if(word.length() == 5 && word.back() == 'f')
      {
         word = word.substr(0,4);
         auto name = getNameColor(word);

         if(name.length() > 0 && word.length() > name.length())
         {
            dataReplace(word, name);
            return {name.length(),nullptr};
         }
      }
      else if(word.length() == 9)
      {
         if(word.ends_with("ff"))
         {
            word = word.substr(0,7);

            if(shortHexRGB(word)) word = word.substr(0,4);

            auto name = getNameColor(word);

            if(name.length() > 0 && word.length() > name.length())
            {
               dataReplace(word, name);
               return {name.length(),nullptr};
            }
         }
         else if(shortHexRGBA(word)) word = word.substr(0,5);
      }
   }
   else if(word == "rgba") return {word.length(), new (heap) RGBATask()};
   else if(word == "rgb") return {word.length(), new (heap) RGBTask()};
   else if(minKeyNameToHex <= word.length() && word.length() <= maxKeyNameToHex)
   {
      auto hex = getHexColor(word);

      if(hex.length() > 0 && word.length() > hex.length())
      {
         dataReplace(word, hex);
         return {hex.length(),nullptr};
      }
   }

   return {word.length(),nullptr};
}

static StreamTask* detectWord(const CSSOptimizer & self,
                              std::string & str,
                              size_t pos_begin,
                              char * heap,
                              StreamTask * task,
                              bool finish = false)
{
   if(!self.hasOptimize()) return nullptr;

   std::string_view word(str.data() + pos_begin, str.length() - pos_begin);

   if(self.optimizeColor())
   {
      if(task != nullptr)
      {
         if(!task->finish(str))
         {
            if(!task->addWord(word)) task->~StreamTask();
            else if(!task->finish(str)) return task;
         }
         else task->~StreamTask();
      }

      auto pair = findColor(word, heap);

      if(pair.first != word.length())
      {
         str.resize(str.size() - (word.length() - pair.first));
         return nullptr;
      }
      else if(pair.second != nullptr)
      {
         pair.second->setPos(pos_begin);
         return pair.second;
      }
   }

   if(self.optomizeBorderNone())
   {
      pos_begin--;

      if(finish && str[pos_begin] == ':')
      {
         if(word == "none" && std::string_view(str.data(),pos_begin) == "border")
         {
            pos_begin++;
            str[pos_begin] = '0';
            pos_begin++;
            str.resize(pos_begin);
         }
      }
   }

   return nullptr;
}

static PropertiesMap streamAnalysis(const CSSOptimizer & self, std::istream & stream)
{
   char heap[maxTaskSize()];
   StreamTask * task = nullptr;

   PropertiesMap map;

   char c;

   bool comment = false, star = false;

   size_t depth = 0,
          spaces_count = 0,
          word_begin = 0;

   std::set<std::string>elements;
   std::string str;

   while(stream.get(c))
   {
      if(!comment && str.length() > 0 && str.back() == '/' && c == '*')
      {
         comment = true;
         continue;
      }

      if(comment)
      {
         if(!star && c == '*')
         {
            star = true;
         }
         else if(star)
         {
            if(c != '/')
            {
               star = false;
            }
            else
            {
               star = false;
               comment = false;
               str.resize(str.length()-1);
            }
         }

         continue;
      }

      if(c == '{')
      {
         if(depth == 0)
         {
            if(str.length() > 0)
            {
               elements.insert(str);
               str.clear();
            }

            spaces_count = 0;
            depth++;

            continue;
         }

         depth++;
      }

      if(c == '}')
      {
         if(depth == 1)
         {
            if(str.length() > 0)
            {
               if(word_begin != 0 && word_begin != str.length()) task = detectWord(self, str, word_begin, heap, task, true);

               if(task != nullptr)
               {
                  task->finish(str);
                  task->~StreamTask();
                  task = nullptr;
               }

               if(map.contains(str))
               {
                  map[str].insert(elements.begin(), elements.end());
                  elements.clear();
               }
               else map[str] = std::move(elements);
            }
            else elements.clear();

            str.clear();

            word_begin = 0;
            spaces_count = 0;
            depth--;

            continue;
         }

         depth--;
      }

      if(depth == 0 && c == ',')
      {
         if(str.length() > 0)
         {
            elements.insert(str);
            str.clear();
         }

         spaces_count = 0;

         continue;
      }

      if(depth == 1 && c == ';')
      {
         if(str.length() > 0)
         {
            if(word_begin != 0 && word_begin != str.length()) task = detectWord(self, str, word_begin, heap, task, true);

            if(task != nullptr)
            {
               task->finish(str);
               task->~StreamTask();
               task = nullptr;
            }

            if(map.contains(str)) map[str].insert(elements.begin(), elements.end());
            else map[str] = elements;

            str.clear();
         }

         word_begin = 0;
         spaces_count = 0;

         continue;
      }

      if(std::isspace(c))
      {
         if(str.length()) spaces_count++;

         continue;
      }

      if(spaces_count > 0)
      {
         if(str.back() != ':' && str.back() != ',' && str.back() != '(' && c != '(' && c != ')')
         {
            if(word_begin != 0 && word_begin != str.length()) task = detectWord(self, str, word_begin, heap, task);
            word_begin = str.length() + 1;

            str.push_back(' ');
         }

         spaces_count = 0;
      }

      if(depth == 1 && (c == ',' || c == '(' || c == ')'))
      {
         if(word_begin != 0 && word_begin != str.length()) task = detectWord(self, str, word_begin, heap, task);
         if(task != nullptr) task->addSeparator(c);
         word_begin = str.length() + 1;
      }

      str.push_back(c);

      if(depth == 1 && c == ':') word_begin = str.length();
   }

   return map;
}

static void uniqueAnalysis(const CSSOptimizer & self, PropertiesMap & pmap, std::ostream & out)
{
   PropertiesMap inverted;

   for(auto begin = pmap.begin(); begin != pmap.end();)
   {
      if(begin->second.size() == 1)
      {
         inverted[*begin->second.begin()].insert(begin->first);
         begin = pmap.erase(begin);
         continue;
      }

      begin++;
   }

   for(auto begin = inverted.begin(); begin != inverted.end();)
   {
      out << begin->first << "{";

      for(bool noFirst = false; const auto & w : begin->second)
      {
         if(noFirst) out << ';';
         else noFirst = true;

         out << w;
      }

      out << '}';

      if(self.lineSeparator()) out << std::endl;

      begin = inverted.erase(begin);
   }
}

static void outMap(const CSSOptimizer & self, PropertiesMap & pmap, std::ostream & out)
{
   for(auto begin = pmap.begin(); begin != pmap.end();)
   {
      for(bool noFirst = false; const auto & w : begin->second)
      {
         if(noFirst) out << ',';
         else noFirst = true;

         out << w;
      }

      out << '{' << begin->first << '}';

      if(self.lineSeparator()) out << std::endl;

      begin = pmap.erase(begin);
   }
}

void CSSOptimizer::optimize(std::istream & in, std::ostream & out)
{
   PropertiesMap pmap = streamAnalysis(*this, in);
   uniqueAnalysis(*this, pmap, out);
   outMap(*this, pmap, out);
}
