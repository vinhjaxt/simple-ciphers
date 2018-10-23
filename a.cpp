#include <iostream>

// arguments parser
#include "libs/cxxopts/include/cxxopts.hpp"

std::string ascii = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

typedef std::tuple<int (*)(std::string), int (*)(std::string)> CipherFunctions;
int shiftCipherEncrypt(std::string);
int substitutionCipherEncrypt(std::string);
int affineCipherEncrypt(std::string);
int vigenereCipherEncrypt(std::string);

int shiftCipherDecrypt(std::string);
int substitutionCipherDecrypt(std::string);
int affineCipherDecrypt(std::string);
int vigenereCipherDecrypt(std::string);

std::map<std::string, CipherFunctions> supportCiphers = {
  {"shift", std::make_tuple(shiftCipherEncrypt, shiftCipherDecrypt)},
  {"substitution", std::make_tuple(substitutionCipherEncrypt, substitutionCipherDecrypt)},
  {"affine", std::make_tuple(affineCipherEncrypt, affineCipherDecrypt)},
  {"vigenere", std::make_tuple(vigenereCipherEncrypt, vigenereCipherDecrypt)},
};

int main(int argc, char* argv[]) {
  try {
    cxxopts::Options options(argv[0], "Simple ciphers");
    options.allow_unrecognised_options().add_options()
      ("e,encrypt", "Encrypt")
      ("d,decrypt", "Decrypt")
      ("t,type", "Cipher type, supports: shift, substitution, affine, vigenere", cxxopts::value<std::string>(), "type")
      ("i,input", "Input string", cxxopts::value<std::string>(), "text")
      ("h,help", "Print help")
    ;
    // options.parse_positional({"input"}); // Vị trí đầu tiên của args: argv[1] sẽ là input ?

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      std::cout << options.help({""}) << std::endl;
      exit(0);
    }

    if (result.count("e") + result.count("d") != 1) {
      std::cout << "You must provide 'd' or 'e' option" << std::endl;
      exit(0);
    }

    if (result.count("input") < 1) {
      std::cout << "You must provide 'input' option" << std::endl;
      exit(0);
    }
    if (result.count("type") < 1) {
      std::cout << "You must provide 'type' option" << std::endl;
      exit(0);
    }

    std::string type = result["type"].as<std::string>();
    if(supportCiphers.count(type) < 1){
      std::cout << "Cipher type: '" << type << "' does not support" << std::endl;
      exit(1);
    }

    std::string input = result["input"].as<std::string>();
    std::cout << "Input string: '" << input << "'" << std::endl;

    CipherFunctions f = supportCiphers[type];
    if(result.count("e") > 0){
      // encrypt
      std::cout << "Encrypt: " << type << std::endl;
      return ((int(*)(std::string))std::get<0>(f))(input);
    }
    // decrypt
    std::cout << "Decrypt: " << type << std::endl;
    return ((int(*)(std::string))std::get<1>(f))(input);
  } catch (const cxxopts::OptionException& e) {
    std::cout << "error options: " << e.what() << std::endl;
    exit(1);
  }

  return 0;
}

// encrypt
int shiftCipherEncrypt(std::string input){
  int m;
  int range = '~' - ' ' + 1;
  std::cout << "Input 0 < m < " << range << ": ";
  std::cin >> m;
  if(m < 1 || m > range){
    printf("provide 0 < m < %d, m = %d\r\n", range, m);
    return 1;
  }

  std::cout << "Output: '";
  for(char& c : input) {
    std::cout << ((char) ( 32 + (m + c - 32) % range ) );
  }
  std::cout << "'" << std::endl;
  return 0;
}
int substitutionCipherEncrypt(std::string input){
  // create random permutation of characters list
  std::string key = ascii;
  std::srand((unsigned int)std::time(0));
  std::random_shuffle(key.begin(), key.end());

  int asciiLength = ascii.length();
  std::map<char, char> keyMap;

  const char* asciiCstr = ascii.c_str();
  const char* keyCstr = key.c_str();
  for(int i=0; i<asciiLength; i++){
    keyMap.insert(std::make_pair(asciiCstr[i], keyCstr[i]));
  }

  std::cout << "KEY: '" << key << "'" << std::endl;

  std::cout << "Output: '";
  for(char& c : input) {
    std::cout << keyMap[c];
  }
  std::cout << "'" << std::endl;

  return 0;
}
int affineCipherEncrypt(std::string input){
  int range = '~' - ' ' + 1;

  int a, b;
  std::cout << "Input 0 < a, b < " << range << ", gcd(a, " << range << ") = 1 " << std::endl;
  std::cin >> a >> b;

  if ((a < 1 || a > range) || (b < 1 || b > range)) {
    printf("provide 0 < a, b < %d, input: a, b = %d, %d\r\n", range, a, b);
    return 1;
  }

  for(int i=2; i <= a; i++){
    if(range % i == 0 && a % i == 0){
      std::cout << "provide a that gcd(a, " << range << ") = 1" << std::endl;
      return 2;
    }
  }

  std::cout << "Output: '";
  for(char& c : input) {
    std::cout << ((char) ( 32 + (b + a*(c - 32)) % range ) );
  }
  std::cout << "'" << std::endl;
  return 0;
}
int vigenereCipherEncrypt(std::string input){
  int range = '~' - ' ' + 1;

  std::string key;
  std::cout << "Input KEY: " << std::endl;
  std::cin >> key;

  const char* keyCstr = key.c_str();
  int keyLength = key.length();

  std::cout << "Output: '";
  int i = 0;
  for(char& c : input) {
    std::cout << ((char) ( 32 + (keyCstr[i % keyLength] + c - 32) % range ) );
    i++;
  }
  std::cout << "'" << std::endl;
  return 0;
}

// decrypt
int shiftCipherDecrypt(std::string input){
  int m;
  int range = '~' - ' ' + 1;
  std::cout << "Input 0 < m < " << range << ": ";
  std::cin >> m;
  if(m < 1 || m > range){
    printf("provide 0 < m < %d, m = %d\r\n", range, m);
    return 1;
  }

  std::cout << "Output: '";
  for(char& c : input) {
    int v = (c - m - 32) % range;
    if (v < 0) v += range;
    std::cout << ((char) ( 32 + v) );
  }
  std::cout << "'" << std::endl;
  return 0;
}
int substitutionCipherDecrypt(std::string input){
  std::string key;
  std::cout << "Input KEY: " << std::endl;
  getline(std::cin, key);

  int asciiLength = ascii.length();

  if (asciiLength != key.length()){
    std::cout << "Invalid KEY length" << std::endl;
    return 1;
  }

  std::map<char, char> keyMap;

  const char* asciiCstr = ascii.c_str();
  const char* keyCstr = key.c_str();
  for(int i=0; i<asciiLength; i++){
    keyMap.insert(std::make_pair(keyCstr[i], asciiCstr[i]));
  }

  std::cout << "Output: '";
  for(char& c : input) {
    std::cout << keyMap[c];
  }
  std::cout << "'" << std::endl;

  return 0;
}
int affineCipherDecrypt(std::string input){
  int range = '~' - ' ' + 1;

  int a, b;
  std::cout << "Input 0 < a, b < " << range << ", gcd(a, " << range << ") = 1 " << std::endl;
  std::cin >> a >> b;

  if ((a < 1 || a > range) || (b < 1 || b > range)) {
    printf("provide 0 < a, b < %d, input: a, b = %d, %d\r\n", range, a, b);
    return 1;
  }

  for(int i=2; i <= a; i++){
    if(range % i == 0 && a % i == 0){
      std::cout << "provide a that gcd(a, " << range << ") = 1" << std::endl;
      return 2;
    }
  }

  for(int i=1; i < range; i++) {
    if((i*a) % range != 1) continue;
    a = i;
  }

  std::cout << "Output: '";
  for(char& c : input) {
    int v = (a*(c - 32 - b)) % range;
    if (v < 0) v += range;
    std::cout << ((char) ( 32 + v) );
  }
  std::cout << "'" << std::endl;
  return 0;
}
int vigenereCipherDecrypt(std::string input){
  int range = '~' - ' ' + 1;

  std::string key;
  std::cout << "Input KEY: " << std::endl;
  std::cin >> key;

  const char* keyCstr = key.c_str();
  int keyLength = key.length();

  std::cout << "Output: '";
  int i = 0;
  for(char& c : input) {
    int v = (c - keyCstr[i % keyLength] - 32) % range;
    if (v < 0) v += range;
    std::cout << ((char) ( 32 + v) );
    i++;
  }
  std::cout << "'" << std::endl;
  return 0;
}
