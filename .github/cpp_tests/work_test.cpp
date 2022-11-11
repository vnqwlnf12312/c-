#include "../../String.cpp"

int main()
{
    String s;
    s += "abc";
    s += s;
    String s1 ("def");
    String s2 (5, 'a');
    String s3 (s2);

    std::cout << s3 << s2 << s1;


    s1 = s1 + s2;
    String s5 = s2 + s3;
    s2 += s[1];
    s3.back();
    s3.front();
    s3 = s3 + s1 + s3;
    s.find ("b");
    s.rfind ("b");
    s.clear();

    s.empty();
    s.length();
    s3.length();
    for (int i = 0; i < 1'000 ; i++) s.push_back('a');
    for (int i = 0; i < 999 ; i++) s.pop_back();

    String s4 ("qwertyuiopasdfghjkl");
    s4.substr (2,5);
}