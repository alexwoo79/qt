#include <cstdio>

struct College { char name[256]; };

void print_name(College* college_ptr) {          // ① 接收 College 指针
    printf("%s College\n", college_ptr->name);   // ② 箭头访问 name,传递给 printf，再次退化
    college_ptr++; // 指针移动到下一个 College
    printf("%s College\n", college_ptr->name);   // 访问下一个 College
    college_ptr++; // 指针移动到下一个 College
    printf("%s College\n", college_ptr->name);   // 访问下一个 College`

}

int main() {
    College best_colleges[] = {"Magdalen", "Nuffield", "Kellogg"};
    print_name(best_colleges);   // ③ 数组退化为指针
}
// 输出：Magdalen College
//       Nuffield College
//       Kellogg College