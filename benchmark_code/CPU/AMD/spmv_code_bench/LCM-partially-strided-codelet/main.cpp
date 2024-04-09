#include "Analyzer.h"
#include "DDT.h"
#include "Executor.h"
#include "GenerateCppSource.h"
#include "Input.h"
#include "Inspector.h"

#include <DDTUtils.h>
#include <algorithm>

int main(int argc, char** argv) {
  // Parse program arguments
  auto config = DDT::parseInput(argc, argv);

  // Allocate memory and generate global object
  auto d = DDT::init(config);

  // Parse into run-time Codelets
  auto cl = new std::vector<DDT::Codelet *>[config.nThread]();
  DDT::inspectSerialTrace(d, cl, config);

  // Execute codes
  if (config.analyze) {
      DDT::analyzeData(d, cl, config);
  } else {
      DDT::executeCodelets(cl, config);
      // DDT::generateSource(d);
  }

  // Clean up
  DDT::free(d);

  return 0;
}
