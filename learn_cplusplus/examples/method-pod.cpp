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
    int year;
};  
int main() {
    ClockOfTheLongNow clock;
    if (!clock.set_year(2018)) {
        clock.set_year(2019);
    }
    clock.add_year();
    printf("Year: %d\n", clock.get_year());
}       