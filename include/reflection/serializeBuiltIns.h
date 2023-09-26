namespace Typhoon::Reflection {

class InputArchive;
class OutputArchive;

bool read(char& data, const InputArchive& archive);
bool read(unsigned char& data, const InputArchive& archive);
bool read(short& data, const InputArchive& archive);
bool read(unsigned short& data, const InputArchive& archive);
bool read(int& data, const InputArchive& archive);
bool read(unsigned int& data, const InputArchive& archive);
bool read(long& data, const InputArchive& archive);
bool read(unsigned long& data, const InputArchive& archive);
bool read(long long& data, const InputArchive& archive);
bool read(unsigned long long& data, const InputArchive& archive);
bool read(bool& data, const InputArchive& archive);
bool read(float& data, const InputArchive& archive);
bool read(double& data, const InputArchive& archive);
bool read(const char*& data, const InputArchive& archive);

void write(bool data, OutputArchive& archive);
void write(char data, OutputArchive& archive);
void write(unsigned char data, OutputArchive& archive);
void write(short data, OutputArchive& archive);
void write(unsigned short data, OutputArchive& archive);
void write(int data, OutputArchive& archive);
void write(unsigned int data, OutputArchive& archive);
void write(long data, OutputArchive& archive);
void write(unsigned long data, OutputArchive& archive);
void write(long long data, OutputArchive& archive);
void write(unsigned long long data, OutputArchive& archive);
void write(float data, OutputArchive& archive);
void write(double data, OutputArchive& archive);
void write(const char* str, OutputArchive& archive);

} // namespace Typhoon::Reflection
