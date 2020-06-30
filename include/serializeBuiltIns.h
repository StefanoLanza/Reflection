namespace Typhoon::Reflection {

class InputArchive;
class OutputArchive;

bool serialize(char& data, InputArchive& archive);
bool serialize(unsigned char& data, InputArchive& archive);
bool serialize(short& data, InputArchive& archive);
bool serialize(unsigned short& data, InputArchive& archive);
bool serialize(int& data, InputArchive& archive);
bool serialize(unsigned int& data, InputArchive& archive);
bool serialize(long& data, InputArchive& archive);
bool serialize(unsigned long& data, InputArchive& archive);
bool serialize(long long& data, InputArchive& archive);
bool serialize(unsigned long long& data, InputArchive& archive);
bool serialize(bool& data, InputArchive& archive);
bool serialize(float& data, InputArchive& archive);
bool serialize(double& data, InputArchive& archive);
bool serialize(const char* data, InputArchive& archive);

bool serialize(bool data, OutputArchive& archive);
bool serialize(char data, OutputArchive& archive);
bool serialize(unsigned char data, OutputArchive& archive);
bool serialize(short data, OutputArchive& archive);
bool serialize(unsigned short data, OutputArchive& archive);
bool serialize(int data, OutputArchive& archive);
bool serialize(unsigned int data, OutputArchive& archive);
bool serialize(long data, OutputArchive& archive);
bool serialize(unsigned long data, OutputArchive& archive);
bool serialize(long long data, OutputArchive& archive);
bool serialize(unsigned long long data, OutputArchive& archive);
bool serialize(float data, OutputArchive& archive);
bool serialize(double data, OutputArchive& archive);
bool serialize(const char* str, OutputArchive& archive);

} // namespace Typhoon::Reflection
