#include <iostream>
#include <fstream>
#include <string>
#include <codecvt>
#include <locale>
#include <sstream>
#include <cstdlib>

#include <Windows.h>

using namespace std;

/*
	reference:
	http://en.cppreference.com/w/cpp/locale/codecvt_mode

*/

locale g_utf8_locale(locale::empty(), new codecvt_utf8<wchar_t, 0x10ffffff, generate_header>);


class Utf8File
{
private:
	wstring	m_text;

public:
	Utf8File(wstring text) : m_text(text) {}

	void write(wstring fileName)
	{
		wofstream wof;
		wof.imbue(g_utf8_locale);
		wof.open(fileName);
		wof << m_text;
		wof.close();
	}

	void read(wstring fileName)
	{
		wifstream wif;
		wif.imbue(g_utf8_locale);
		wif.open(fileName);
		wstringstream wss;
		wss << wif.rdbuf();
		m_text = wss.str();
	}

	wstring getText() { return m_text; }
	void setText(wstring text) { m_text = text; }
};

wstring method1(string fileName)
{
	locale utf8_locale(locale::empty(), new codecvt_utf8<wchar_t, 0x10ffffff, generate_header>);

	wifstream wif;
	wif.imbue(utf8_locale);
	wif.open("plik.txt");

	if (!wif.is_open())
	{
		stringstream ss;
		ss << "Can't open file " + fileName;
		throw exception((string("Cant't open file ") + fileName).c_str());	// ss.str().c_str()
	}
	
	wstringstream wss;
	wss << wif.rdbuf();
	
	return wss.str();
}

string to_utf8(const wchar_t* buffer, int len)
{
	int nChars = ::WideCharToMultiByte(CP_UTF8,0,buffer,len,NULL,0,NULL,NULL);
	if (nChars == 0) return "";

	string newbuffer;
	newbuffer.resize(nChars);

	::WideCharToMultiByte(CP_UTF8,0,buffer,len,const_cast< char* >(newbuffer.c_str()),nChars,NULL,NULL);

	return newbuffer;
}

void writeToUtf8File(wstring fileName, wstring text)
{
	wofstream wof;
	wof.imbue(g_utf8_locale);
	wof.open(fileName);
	wof << text;
	wof.close();
}

wstring readFromUtf8File(wstring fileName)
{
	wifstream wif;
	wif.imbue(g_utf8_locale);
	wif.open(fileName);
	wstringstream wss;
	wss << wif.rdbuf();
	return wss.str();
}

// Use Windows specific funtion to read wchar_t characters from file
// _wfopen
// _wstat
// fread
std::wstring LoadUtf8FileToString(const std::wstring& filename)
{
	std::wstring buffer;
	
	FILE* f;
	_wfopen_s(&f, filename.c_str(), L"rtS, ccs=UTF-8");

	if (f == NULL)
	{
		// ERROR
		return buffer;
	}

	struct _stat fileinfo;
	_wstat(filename.c_str(), &fileinfo);
	size_t filesize = fileinfo.st_size;

	// Read entire file contents in to memory
	if (filesize > 0)
	{
		buffer.resize(filesize);
		cout << "filesize: " << filesize << endl;
		wcout << "buffer size: " << buffer.size() << endl;
		
		size_t wchars_read = fread(&(buffer.front()), sizeof(wchar_t), filesize, f);
		
		cout << "wchars_read: " << wchars_read << endl;
		buffer.resize(wchars_read);
		wcout << "buffer size: " << buffer.size() << endl;
		buffer.shrink_to_fit();
		wcout << "buffer after shrink size: " << buffer.size() << endl;
	}

	fclose(f);

	return buffer;
}


int main(int argc, char** argv)
{
	cout << argv[0] << endl;

	char plainString[] = "Hello";		//Local encoding, whatever that may be.
	wchar_t wideString[] = L"Hello";	//Wide characters, usually UTF-16 or UTF-32.
	char utf8String[] = u8"Hello";		//UTF-8 encoding.
	char16_t utf16String[] = u"Hello";	//UTF-16 encoding.
	char32_t utf32String[] = U"Hello";	//UTF-32 encoding.

	try
	{
		/*wstring text = L"To jest tekst. £ódŸ to piêkne polskie miasto.";
		writeToUtf8File(L"asd.txt", text);
		wcout << text << endl;
		text = readFromUtf8File(L"asd.txt");
		wcout << text << endl;
		*/

		//while (getline(wifs, txtline))
		//	wcout << ++c << L'\t' << txtline << L'\n';

		wstring text = LoadUtf8FileToString(L"asd.txt");
		wcout << text << endl;
	}
	catch (const runtime_error& e)
	{
		cerr << "Error: " << e.what() << endl;
	}
	catch (const exception& e)
	{
		cerr << "Exception: " << e.what() << endl;
	}
	
	// wcout << L"To jest tekst polsk¹ czcionk¹. £ñæŸ¿¹ów." << endl;

	system("PAUSE");
	return 0;
}