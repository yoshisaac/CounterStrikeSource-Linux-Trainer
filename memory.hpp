#pragma once
#include <sys/uio.h>
#include <iostream>
#include <cerrno>
#include <sstream>
#include <fstream>
#include <cstddef>

namespace Memory {
  //Write memory
  inline bool Write(pid_t pid, unsigned int address, void *buf, size_t size) {
    struct iovec local[1];
    struct iovec remote[1];

    local[0].iov_base = buf;
    local[0].iov_len = size;
    remote[0].iov_base = (void*)address;
    remote[0].iov_len = size;

    if (process_vm_writev(pid, local, 1, remote, 1, 0) == -1) {
      std::cout << "Error write: " << errno << '\n';
      std::cout << "Error write address: " << std::hex << address << '\n';
    }

    return (process_vm_writev(pid, local, 1, remote, 1, 0) == size);
  }

  //Read memory
  inline bool Read(pid_t pid, unsigned int address, void *buf, size_t size) {
    struct iovec local[1];
    struct iovec remote[1];

    local[0].iov_base = buf;
    local[0].iov_len = size;
    remote[0].iov_base = (void*)address;
    remote[0].iov_len = size;

    if (process_vm_readv(pid, local, 1, remote, 1, 0) == -1) {
      std::cout << "Error read: " << errno << '\n';
      std::cout << "Error read address: " << std::hex << address << '\n';
    }
  
    return (process_vm_readv(pid, local, 1, remote, 1, 0) == size);
  }

  inline unsigned int hexToInt(const std::string &hexString) {
    // Use stringstream for conversion
    std::stringstream ss;
    ss << std::hex << hexString;

    // Store the result
    unsigned int result;
    ss >> result;

    return result;
  }
  
  inline unsigned int getModuleBaseAddress(pid_t procPid, std::string moduleName) {
    
    std::ifstream infile(("/proc/" + std::to_string(procPid) + "/maps").c_str());
    std::string line;

    bool found = false;
  
    std::string concatTemp;
    while (std::getline(infile, line)) {
      for (int i = 0; i <= line.length(); i++) {
	concatTemp = "";
	for (int h = 0; h < moduleName.length(); h++) {
	  concatTemp += line[i+h];
	}
	if (concatTemp == moduleName) { found = true; break; }
      }

      if (found == true) {break;}
    }

    concatTemp = "";
    for (int i = 0; i <= line.length(); i++) {
      if (line[i] == '-') { break; }
      concatTemp += line[i];
    }
  
    return hexToInt("0x"+concatTemp);
  }

  /*
  inline unsigned int getModuleSize(pid_t procPid, std::string moduleName) {
    
    std::ifstream infile(("/proc/" + std::to_string(procPid) + "/maps").c_str());
    std::string line;

    std::string found[256];
    int foundIter = 0;
  
    std::string concatTemp;
    while (std::getline(infile, line)) {
      for (int i = 0; i <= line.length(); i++) {
	concatTemp = "";
	for (int h = 0; h < moduleName.length(); h++) {
	  concatTemp += line[i+h];
	}
	if (concatTemp == moduleName) { found[foundIter] = line; foundIter++; }
      }
    }

    concatTemp = "";
    std::string concatTemp2 = "";
    int curIter = 1;
    int i;
    for (std::string s : found) {
      if (s == "") { continue; }

      if (curIter == 1) {
	for (i = 0; i < s.length(); i++) {
	  if (s[i] == '-') { break; }
	  concatTemp += s[i];
	}
      }

      if (curIter == foundIter) {
	for (int h = i+1; h < s.length(); h++) {
	  if (s[h] == ' ') { break; }
	  concatTemp2 += s[h];
	}
      }
      
      curIter++;
    }

    return (hexToInt("0x"+concatTemp2) - hexToInt("0x"+concatTemp));
  }
  */
};
