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
    ClockOfTheLongNow* clock_ptr = &clock;
    clock_ptr->set_year(2020);//
    printf("Address of clock: %p\n",clock_ptr);
    printf("Year: %d\n", clock_ptr->get_year());

}       