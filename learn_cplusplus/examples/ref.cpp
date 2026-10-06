#include <cstdio>

struct ClockOfTheLongNow {
    void add_year() {
        year++;
    }
    bool set_year(int y) {
        if (y <2019) {
            return false;
        }
        year = y;
        return true;
    }
    int get_year() {
        return year;
    }
    private:
    int year{2019};//初始化为2019年
};  


void add_year(ClockOfTheLongNow& clock) {     // ① 引用参数
    clock.set_year(clock.get_year() + 1);     // ② 无需解引用
}

int main() {
    ClockOfTheLongNow clock;
    printf("The year is %d.\n", clock.get_year());  // ③ 2019
    add_year(clock);                                 // ④ 直接传对象
    printf("The year is %d.\n", clock.get_year());  // ⑤ 2020
}