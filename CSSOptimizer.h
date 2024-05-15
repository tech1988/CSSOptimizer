#ifndef CSSOPTIMIZER_H
#define CSSOPTIMIZER_H

#if defined(SHARED_LIB) && defined(WIN32)
#define MODULE __declspec(dllexport)
#else
#define MODULE
#endif

#include <iostream>

class MODULE CSSOptimizer
{
  bool _lineSeparator = false,
       _optimize = true,
       _optimizeColor = true,
       _optomizeBorderNone = true;

  void checkOptimize(){ _optimize = _optimizeColor & _optomizeBorderNone; }

public:
  bool lineSeparator() const { return _lineSeparator; }
  void setLineSeparator(bool enable){ _lineSeparator = enable; }

  bool hasOptimize() const { return _optimize; }

  bool optimizeColor() const { return _optimizeColor; }
  void setOptimizeColor(bool enable)
  {
      _optimizeColor = enable;
      checkOptimize();
  }

  bool optomizeBorderNone() const { return _optomizeBorderNone; }
  void setOptimizeBorderNone(bool enable)
  {
      _optomizeBorderNone = enable;
      checkOptimize();
  }

  void optimize(std::istream & in, std::ostream & out);
};

#endif // CSSOPTIMIZER_H
