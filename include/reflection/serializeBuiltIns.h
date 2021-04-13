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

bool write(bool data, OutputArchive& archive);
bool write(char data, OutputArchive& archive);
bool write(unsigned char data, OutputArchive& archive);
bool write(short data, OutputArchive& archive);
bool write(unsigned short data, OutputArchive& archive);
bool write(int data, OutputArchive& archive);
bool write(unsigned int data, OutputArchive& archive);
bool write(long data, OutputArchive& archive);
bool write(unsigned long data, OutputArchive& archive);
bool write(long long data, OutputArchive& archive);
bool write(unsigned long long data, OutputArchive& archive);
bool write(float data, OutputArchive& archive);
bool write(double data, OutputArchive& archive);
bool write(const char* str, OutputArchive& archive);

} // namespace Typhoon::Reflection
