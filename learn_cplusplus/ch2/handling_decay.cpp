#include <cstdio>

struct College { char name[256]; };

void print_names(College* colleges, size_t n_colleges) {  // ① 指针
    for (size_t i = 0; i < n_colleges; i++) {             // ③ 遍历 0..n-1
        printf("%s College\n", colleges[i].name);         // ④ 下标 + 成员
    }
}

int main() {
    College oxford[] = {"Magdalen", "Nuffield", "Kellogg"};
    print_names(oxford, sizeof(oxford) / sizeof(College)); // ② 传入长度
}