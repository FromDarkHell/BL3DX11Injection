#pragma once

#include <fstream>

namespace Logging {
	std::wofstream myfile;

	void Open() {
		myfile.open("C:\\log.txt", std::ios_base::app);
	}

	void Close() {
		myfile.close();
	}
}