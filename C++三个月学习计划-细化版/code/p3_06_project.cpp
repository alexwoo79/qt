// 例题 3-6：综合项目骨架 —— 多态成绩管理（vector + 智能指针 + 文件 + RAII）
// 每个设计点都对应前面某道例题：多态=3-1、所有权=2-4、异常安全=2-3
#include <cstdio>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class Student {
  public:
    Student(std::string name, double base) : name_(std::move(name)), base_(base) {
        printf("  构造 %-8s 对象 @ %p\n", name_.c_str(), (void *)this);
    }
    virtual ~Student() { printf("  析构 %-8s @ %p\n", name_.c_str(), (void *)this); }

    virtual double finalScore() const = 0;   // 纯虚：算法由子类决定
    virtual const char *kind() const = 0;
    virtual std::string describe() const {   // 基类实现里调用虚函数 → 多态
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%-8s %-8s 卷面 %5.1f → 总评 %5.1f",
                 name_.c_str(), kind(), base_, finalScore());
        return buffer;
    }
    const std::string &name() const { return name_; }
    double baseScore() const { return base_; }

  protected:
    std::string name_;
    double base_;
};

class Regular : public Student {
  public:
    using Student::Student;
    double finalScore() const override { return base_; }
    const char *kind() const override { return "普通生"; }
};

class Honors : public Student {
  public:
    using Student::Student;
    double finalScore() const override { return base_ * 1.15; }   // 加分政策
    const char *kind() const override { return "优等生"; }
};

class Makeup : public Student {
  public:
    using Student::Student;
    double finalScore() const override { return base_ * 0.9; }    // 补考折算
    const char *kind() const override { return "补考生"; }
};

class GradeBook {
  public:
    void add(std::unique_ptr<Student> student) { students_.push_back(std::move(student)); }

    void report() const {
        printf("-- 名册（按对象真实类型多态求值）--\n");
        for (const auto &student : students_) {
            printf("  %s\n", student->describe().c_str());
        }
        if (!students_.empty()) {
            printf("  平均总评 = %.2f\n", average());
            void **vptr = *reinterpret_cast<void ***>(students_.front().get());
            printf("  首个对象的 vptr = %p（基类指针 + 虚表 → 多态的机械本质）\n", (void *)vptr);
        }
    }

    double average() const {
        if (students_.empty()) return 0.0;
        double sum = 0.0;
        for (const auto &student : students_) sum += student->finalScore();
        return sum / static_cast<double>(students_.size());
    }

    bool save(const std::string &path) const {
        std::ofstream out(path);                 // RAII：离开函数自动关闭
        if (!out) return false;
        for (const auto &student : students_) {
            out << student->kind() << ' ' << student->name() << ' ' << student->baseScore() << '\n';
        }
        return static_cast<bool>(out);
    }

    bool load(const std::string &path) {
        std::ifstream in(path);
        if (!in) return false;
        students_.clear();
        std::string kind, name;
        double base = 0.0;
        while (in >> kind >> name >> base) {
            if (kind == "优等生") add(std::make_unique<Honors>(name, base));
            else if (kind == "补考生") add(std::make_unique<Makeup>(name, base));
            else add(std::make_unique<Regular>(name, base));
        }
        return true;
    }

  private:
    std::vector<std::unique_ptr<Student>> students_;  // 独占所有权，容器只移动
};

int main() {
    printf("sizeof(Regular)=%zu sizeof(Honors)=%zu sizeof(Makeup)=%zu（都含 vptr）\n\n",
           sizeof(Regular), sizeof(Honors), sizeof(Makeup));

    const std::string path = "/tmp/cpp_gradebook.txt";
    {
        GradeBook book;
        printf("-- 构建名册 --\n");
        book.add(std::make_unique<Honors>("Tom", 85));
        book.add(std::make_unique<Regular>("Jerry", 78));
        book.add(std::make_unique<Makeup>("Spike", 60));
        printf("\n");
        book.report();
        printf("\n-- 持久化（ofstream RAII，无需手动 close）--\n");
        printf("  保存到 %s：%s\n", path.c_str(), book.save(path) ? "成功" : "失败");
        printf("\n-- 读回来（工厂 + unique_ptr 重新建对象）--\n");
        GradeBook reloaded;
        if (reloaded.load(path)) {
            printf("\n");
            reloaded.report();
        }
    }   // ← 两个 GradeBook 在这里析构：vector 析构 → unique_ptr 析构 → 虚析构 → 完整释放
    printf("\n所有对象已按构造的逆序释放，没有裸 new/delete，也没有泄漏。\n");
    return 0;
}
