# Midterm Project – Implement `ls(1)`

---

**Sinh viên thực hiện:** Chu Ngọc Hoàng   
**Mã sinh viên:** 23IT082  

---

## 1. Giới thiệu

Dự án **`my_ls`** là phiên bản rút gọn và tự cài đặt của lệnh **`ls(1)`** trong hệ điều hành UNIX.  
Chương trình được viết hoàn toàn bằng ngôn ngữ **C**, nhằm mục tiêu giúp sinh viên hiểu sâu hơn về **hệ thống tập tin UNIX**, **thao tác với thư mục**, và **lập trình cấp hệ thống (system-level programming)**.  

---

## 2. Mục tiêu của project

- Hiểu cách hệ điều hành quản lý và truy cập hệ thống tập tin.  
- Rèn luyện kỹ năng thao tác với **struct `stat`**, **thư viện `<dirent.h>`**, và các hàm **system call**.  
- Tự cài đặt các tùy chọn cơ bản của `ls` như:
  - Liệt kê thư mục, tệp tin.
  - Hiển thị file ẩn (`-a`).
  - Hiển thị chi tiết (`-l`, `-h`).
  - Hiển thị inode, block, UID/GID, phân loại ký hiệu (`-i`, `-s`, `-n`, `-F`).
  - Sắp xếp theo thời gian, kích thước (`-t`, `-S`, `-r`).
  - Đệ quy thư mục con (`-R`).
  - Xử lý lỗi, thư mục rỗng, quyền truy cập.  
- Áp dụng mô hình **lập trình module hóa**, chia nhỏ project thành nhiều file `.c` và `.h`.

---

## 3. Mức độ hoàn thiện

| Tính năng | Trạng thái | Ghi chú |
|------------|-------------|---------|
| Liệt kê thư mục & tệp tin |  Hoàn thiện | Hiển thị chính xác nội dung |
| File ẩn (`-a`, `-A`) |  Hoàn thiện | Đúng hành vi UNIX |
| Hiển thị chi tiết (`-l`, `-h`, `-n`) |  Hoàn thiện | Có quyền, UID/GID, kích thước dễ đọc |
| Phân loại ký hiệu (`-F`) |  Hoàn thiện | Hiển thị `/`, `*`, `@` đúng chuẩn |
| Sắp xếp (`-S`, `-t`, `-r`, `-f`) |  Hoàn thiện | Đúng thứ tự, ổn định |
| Đệ quy (`-R`) |  Hoàn thiện | Hiển thị đầy đủ thư mục con |
| Xử lý lỗi truy cập, thư mục rỗng |  Hoàn thiện | Không segfault |
| Tích hợp Makefile |  Hoàn thiện | Biên dịch nhanh gọn |
| README và báo cáo |  Hoàn thiện | Phục vụ nộp bài & GitHub |

---

## 4. Cấu trúc thư mục dự án

```
midterm_ls/
│
├── include/               # Header files (.h)
│   ├── file_info.h
│   ├── list_dir.h
│   ├── options.h
│   └── utils.h
│
├── src/                   # Source files (.c)
│   ├── file_info.c        # Xử lý thông tin và định dạng hiển thị file
│   ├── list_dir.c         # Liệt kê thư mục, xử lý -R, -a, -l, ...
│   ├── options.c          # Phân tích tham số dòng lệnh (flag)
│   ├── utils.c            # Hàm phụ trợ: sắp xếp, nối chuỗi, kiểm tra lỗi
│   └── main.c             # Hàm main, điều phối toàn bộ chương trình
│
├── Makefile               # Tự động biên dịch project
└── README.md              # Tài liệu hướng dẫn (file này)
```

---

## 5. Mô tả và phân tích các module chính

Dự án được chia thành nhiều module độc lập, mỗi module đảm nhiệm một vai trò cụ thể trong quy trình xử lý lệnh `ls`.  
Cách chia tách này giúp code dễ đọc, dễ bảo trì, và tuân thủ mô hình **modular programming**.

---

### 5.1 `main.c` — Điểm khởi đầu chương trình

**Chức năng:**  
- Đọc tham số dòng lệnh.  
- Gọi module `options.c` để xử lý flag.  
- Gọi `list_directory()` để bắt đầu hiển thị nội dung thư mục.  

**Đoạn code tiêu biểu:**
```c
int main(int argc, char *argv[]) {
    Options opt = parse_options(argc, argv);
    const char *path = (opt.path) ? opt.path : ".";
    list_directory(path, opt);
    return 0;
}
```

**Phân tích:**  
- Hàm `parse_options()` xử lý toàn bộ tham số truyền vào (ví dụ `-alhR`).  
- Mặc định, nếu người dùng không chỉ định đường dẫn, chương trình sẽ liệt kê thư mục hiện tại `"."`.  
- Mọi xử lý hiển thị đều được ủy quyền cho module `list_dir.c`.  

---

### 5.2 `options.c` — Phân tích và lưu cờ tùy chọn (flags)

**Chức năng:**  
- Nhận mảng `argv[]` từ `main.c`.  
- Dò từng ký tự trong các flag để bật/tắt các tùy chọn trong struct `Options`.  

**Đoạn code tiêu biểu:**
```c
Options parse_options(int argc, char *argv[]) {
    Options opt = {0};
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j]; j++) {
                switch (argv[i][j]) {
                    case 'a': opt.show_all = 1; break;
                    case 'l': opt.long_format = 1; break;
                    case 'R': opt.recursive = 1; break;
                    case 'h': opt.human = 1; break;
                    case 't': opt.sort_time = 1; break;
                    default:  fprintf(stderr, "Unknown option: -%c\n", argv[i][j]);
                }
            }
        } else opt.path = argv[i];
    }
    if (!opt.path) opt.path = ".";
    return opt;
}
```

**Phân tích:**  
- Cấu trúc `Options` lưu các cờ như `-a`, `-l`, `-R` bằng biến boolean.  
- Cho phép kết hợp nhiều flag trong một tham số (ví dụ `-alh`).  
- Giúp chương trình tách biệt rõ phần “giao tiếp người dùng” và phần “xử lý hiển thị”.

---

### 5.3 `list_dir.c` — Liệt kê thư mục và duyệt đệ quy

**Chức năng:**  
- Mở thư mục (`opendir()`), đọc từng entry (`readdir()`),  
  bỏ qua hoặc hiển thị tùy theo flag `-a`, `-A`.  
- Nếu có `-R`, gọi đệ quy để xử lý thư mục con.  

**Đoạn code tiêu biểu:**
```c
void list_directory(const char *path, Options opt) {
    DIR *dir = opendir(path);
    if (!dir) { perror("ls"); return; }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!opt.show_all && entry->d_name[0] == '.') continue;
        print_file_info(path, entry->d_name, &opt);
    }

    if (opt.recursive) {
        rewinddir(dir);
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR &&
                strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
                char subpath[512];
                snprintf(subpath, sizeof(subpath), "%s/%s", path, entry->d_name);
                printf("\n%s:\n", subpath);
                list_directory(subpath, opt);
            }
        }
    }

    closedir(dir);
}
```

**Phân tích:**  
- Sử dụng `struct dirent` để truy xuất tên và loại đối tượng trong thư mục.  
- Kiểm tra `entry->d_type` để xác định thư mục (`DT_DIR`) cho đệ quy.  
- Gọi `print_file_info()` để hiển thị chi tiết từng tệp tin.  
- Xử lý cẩn thận với `.` và `..` để tránh vòng lặp vô hạn khi đệ quy.  

---

### 5.4 `file_info.c` — Hiển thị thông tin chi tiết file

**Chức năng:**  
- Lấy thông tin file bằng `lstat()`.  
- In quyền truy cập, liên kết, UID, GID, kích thước, và thời gian.  
- Hỗ trợ hiển thị ký hiệu phân loại (`-F`).  

**Đoạn code tiêu biểu:**
```c
void print_file_info(const char *path, const char *filename, Options opt) {
    struct stat st;
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, filename);
    if (lstat(fullpath, &st) < 0) return;

    if (opt.long_format) {
        print_permissions(st.st_mode);
        printf(" %3ld ", st.st_nlink);

        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        printf("%-8s %-8s %8ld ", 
               pw ? pw->pw_name : "?", 
               gr ? gr->gr_name : "?", 
               st.st_size);

        char timebuf[32];
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
        printf("%s %s\n", timebuf, filename);
    } else printf("%s  ", filename);
}
```

**Phân tích:**  
- `lstat()` thay vì `stat()` để giữ nguyên thông tin symbolic link.  
- `getpwuid()` và `getgrgid()` ánh xạ UID/GID sang tên người dùng và nhóm.  
- Dùng `strftime()` để định dạng thời gian giống `ls -l` thật.  
- Nếu bật flag `-h`, kích thước sẽ được in theo đơn vị KB, MB qua hàm `human_readable()`.  

---

### 5.5 `utils.c` — Hàm tiện ích và xử lý phụ trợ

**Chức năng:**  
- Cung cấp các hàm hỗ trợ cho `list_dir.c` và `file_info.c`.  
- Bao gồm: nối chuỗi đường dẫn, so sánh để sắp xếp, chuyển kích thước sang định dạng dễ đọc.  

**Đoạn code tiêu biểu:**
```c
void human_readable(off_t size, char *buf) {
    const char *units[] = {"B","K","M","G"};
    double s = (double)size;
    int i = 0;
    while (s >= 1024 && i < 3) { s /= 1024; i++; }
    sprintf(buf, "%.1f%s", s, units[i]);
}
```

**Phân tích:**  
- Giúp hiển thị kích thước file thân thiện với người dùng (`ls -lh`).  
- Các hàm khác trong module này như `join_path()`, `compare_by_size()`… được sử dụng để sắp xếp khi có flag `-S` hoặc `-t`.  
- Giữ cho mã trong các module chính gọn gàng và tách biệt rõ phần xử lý logic phụ.  

---

### 5.6 `Makefile` — Quản lý quá trình biên dịch

**Chức năng:**  
- Tự động hóa việc biên dịch toàn bộ mã nguồn và tạo tệp thực thi.  

**Đoạn code tiêu biểu:**
```makefile
CC = gcc
CFLAGS = -Wall -Iinclude
OBJ = src/main.o src/utils.o src/list_dir.o src/file_info.o src/options.o

my_ls: $(OBJ)
	$(CC) $(CFLAGS) -o my_ls $(OBJ)

clean:
	rm -f src/*.o my_ls
```

**Phân tích:**  
- `-Iinclude` giúp trình biên dịch nhận diện tất cả header `.h` trong thư mục `include/`.  
- Cấu trúc Makefile gọn gàng, dễ hiểu và đáp ứng yêu cầu “project phải có Makefile”.  


---

## 6. Build và khởi động dự án

### Biên dịch chương trình
```bash
make
```

Sau khi chạy, file thực thi `my_ls` sẽ được tạo trong thư mục gốc.

### Chạy chương trình
```bash
./my_ls [OPTIONS] [PATH]
```

**Ví dụ:**
```bash
./my_ls
./my_ls -a
./my_ls -lh
./my_ls -R
./my_ls -alhR /etc
```

---

## 7. Minh họa các Flag

| **Lệnh** | **Ý nghĩa** | **Minh họa (Screenshot)** |
|-----------|--------------|-----------------------------|
| `./my_ls` | Liệt kê tệp và thư mục trong thư mục hiện tại |<img width="514" height="77" alt="image" src="https://github.com/user-attachments/assets/bcd49a4d-37fc-4976-a12b-239c309ca36f" />|
| `./my_ls -a` | Hiển thị tất cả tệp (bao gồm tệp ẩn `.` và `..`) | <img width="521" height="76" alt="image" src="https://github.com/user-attachments/assets/beba3852-0ce5-4644-852e-70b3fb10a5c0" />|
| `./my_ls -A` | Giống `-a` nhưng bỏ qua `.` và `..` | <img width="474" height="76" alt="image" src="https://github.com/user-attachments/assets/ecfcbc54-8f1d-41e7-9a13-621e17651eca" />|
| `./my_ls -l` | Hiển thị chi tiết từng tệp: quyền, số liên kết, chủ sở hữu, nhóm, kích thước, thời gian |<img width="610" height="145" alt="image" src="https://github.com/user-attachments/assets/d0df259a-ee2c-4dee-ad87-d103bbf9b46f" />|
| `./my_ls -lh` | Giống `-l` nhưng kích thước dễ đọc (KB, MB) |<img width="638" height="147" alt="image" src="https://github.com/user-attachments/assets/81a374bc-c194-4017-920b-7c4954b1bd21" />|
| `./my_ls -R` | Đệ quy – liệt kê toàn bộ thư mục con | <img width="875" height="193" alt="image" src="https://github.com/user-attachments/assets/49fbd6bb-4df8-4c9d-bd9e-1b1c367a92a7" />|
| `./my_ls -F` | Phân loại ký hiệu: `/` (thư mục), `*` (thực thi), `@` (liên kết) | <img width="410" height="46" alt="image" src="https://github.com/user-attachments/assets/eef6afc7-91aa-4387-958e-8ef385310a79" />|
| `./my_ls -t` | Sắp xếp theo thời gian chỉnh sửa (mới nhất trước) | <img width="445" height="77" alt="image" src="https://github.com/user-attachments/assets/8e7a3f0a-2f8e-461b-8841-9bf81e5e6498" />|
| `./my_ls -S` | Sắp xếp theo kích thước tệp (lớn nhất trước) | <img width="464" height="74" alt="image" src="https://github.com/user-attachments/assets/d1036c7a-a0dd-4a7c-8376-0ddb573c3030" />|
| `./my_ls -r` | Đảo ngược thứ tự sắp xếp | <img width="456" height="73" alt="image" src="https://github.com/user-attachments/assets/0f881f70-6cd5-4596-9983-eb37842e7686" />|
| `./my_ls -n` | Hiển thị UID/GID dạng số thay vì tên | <img width="404" height="108" alt="image" src="https://github.com/user-attachments/assets/28269a77-66cc-4861-99c3-dbc1d775e861" />|
| `./my_ls -is` | In thêm số inode và block của tệp | <img width="872" height="89" alt="image" src="https://github.com/user-attachments/assets/1baa09d7-c605-4df9-9e6d-5dd32c288d21" />|
| `./my_ls -alhR` | Kết hợp nhiều tùy chọn cùng lúc (ẩn + chi tiết + dễ đọc + đệ quy) | <img width="677" height="523" alt="image" src="https://github.com/user-attachments/assets/aca4b5ed-b846-47cc-b000-d01b06f1814a" />|

---

## 8. Kết luận

Dự án **my_ls** đã mô phỏng chính xác hành vi cơ bản của lệnh `ls` trên UNIX/Linux.  
Chương trình được chia module rõ ràng, có tính ổn định, không gặp lỗi phân đoạn (segfault), và dễ mở rộng cho các tính năng khác trong tương lai.  

Dự án thể hiện:
- Kỹ năng lập trình hệ thống (System Programming).  
- Hiểu biết về cấu trúc file, quyền truy cập, và thao tác thư mục.  
- Tuân thủ quy tắc viết code sạch, dễ bảo trì và có tính mở rộng.





