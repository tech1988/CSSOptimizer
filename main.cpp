#include "CSSOptimizer.h"
#include <map>
#include <fstream>
#include <functional>

int main(int argc, char * argv[])
{
    if(argc < 2)
   {
       std::cout << "Incorrect number of arguments" << std::endl;
       return 1;
   }

   CSSOptimizer op;

   std::map<std::string_view, std::function<void()>> flags =
   {
      {"--enable-line-separator", [&op](){ op.setLineSeparator(true); }},
      {"--disable-optimize-color", [&op](){ op.setOptimizeColor(false); }},
      {"--disable-optomize-border-none", [&op](){ op.setOptimizeBorderNone(false); }}
   };

   if(argc == 2)
   {
      if(argv[1] == std::string_view("-h"))
      {
         std::cout << "<input css file> <output css file> <option flags...>" << std::endl
                   << "Option flags:" << std::endl;

         for(const auto & p : flags) std::cout << p.first << std::endl;

         return 0;
      }
      else if(argv[1] == std::string_view("-v"))
      {
         std::cout << "CSSOptimizer -v 0.0.1" << std::endl
                   << "Author: Zmicier Fedarenka" << std::endl;

         return 0;
      }

      std::cout << "Incorrect argument: " << argv[1] << std::endl;

      return 1;
   }

   if(argc > 3)
   {
      for(int i = 3; i < argc; i++)
      {
          auto c = flags[argv[i]];

          if(c == nullptr)
          {
             std::cout << "Incorrect flag: " << argv[i] << std::endl;
             return 1;
          }

          c();
      }
   }

   std::ifstream in(argv[1]);

   if(!in.is_open())
   {
      std::cout << "Cannot open input file:" << argv[1] << std::endl;

      return 1;
   }

   std::ofstream out(argv[2]);

   if(!out.is_open())
   {
       std::cout << "Cannot open output file:" << argv[2] << std::endl;

       return 1;
   }

   op.optimize(in,out);

   return 0;
}
