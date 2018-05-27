/*Chương trình application mang tên helloworld_app.c*/ 
/*Khai báo các thưviện cần thiết cho các hàm sửdụng trong chương 
trình*/
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <linux/ioctl.h> 
/*Định nghĩa các số định danh lệnh cho giao diện ioctl, các số định nghĩa này giống như trong driver*/ 
#define HELLOWORLD_DEV_MAGIC 'B' 
/*Số định danh lệnh dùng cho lệnh add*/ 
#define HELLOWORLD_ADD _IOWR(HELLOWORLD_DEV_MAGIC, 10, unsigned long) 
/*Số định danh lệnh dùng cho lệnh sub*/ 
#define HELLOWORLD_SUB _IOWR(HELLOWORLD_DEV_MAGIC, 11, unsigned long) 
/*Số định danh lệnh dùng cho lệnh mul*/ 
#define HELLOWORLD_MUL _IOWR(HELLOWORLD_DEV_MAGIC, 12, unsigned long) 
/*Số định danh lệnh dùng cho lệnh div*/ 
#define HELLOWORLD_DIV _IOWR(HELLOWORLD_DEV_MAGIC, 13, unsigned long) 
/*Chương trình con in ra hướng dẫn cho người dùng*/ 
void print_usage() 
{ 
	printf("helloworld_app add|sub|mul|div|read|write arg_1 (arg_2)\n"); 
	exit(0); 
} 
/*Hàm main thực thi khi khi chương trình được gọi từ shell, main khai 
báo theo dạng tùy chọn tham số*/ 
int main(int argc, char **argv) 
{  /*Biến lưu số file descripition của driver khi được mở*/
	int fd; 
	/*Biến lưu mã lỗi trả về cho hàm main*/ 
	 int ret = 0; 
	 /*Bộ đệm dữ liệu nhận được từdriver*/ 
	char read_buf[1]; 
	/*Bộ đệm dữ liệu cần truyền sang driver*/ 
	char write_buf[1]; 
	/*Bộ đệm dữ liệu 2 chiều dùng trong hàm giao diện ioctl()*/ 
	 unsigned long arg[3]; 
	/*Đầu tiên mở driver cần tương tác, quy định chế độ truy xuất driver là đọc và ghi*/ 
	 if ((fd = open("/dev/my_module", O_RDWR)) < 0) 
	{ 
		/*Nếu có lỗi trong quá trình mở thiết bị, in ra thông báo cho người dùng*/ 
		printf("Error whilst opening /dev/my_module\n"); 
		/*Trảvềmã lỗi cho hàm main*/ 
		return fd; 
	} 
	/*Liệt kê các trường hợp có thểxảy ra khi giao tiếp với người 
	dùng*/ 
	if (argc == 4) { 
		/*Trong trường hợp là các hàm toán học, dùng ioctl, kiểm tra các phép toán và thực hiện tính toán*/ 
		/*Trong trường hợp là phép toán cộng*/ 
		if (!strcmp(argv[1],"add")) { 
			arg[0] = atoi(argv[2]); 
			arg[1] = atoi(argv[3]); 
			ioctl (fd, HELLOWORLD_ADD, arg); 
			printf ("User: %ld + %ld = %ld\n", arg[0], arg[1], 
			arg[2]); 
	} 
	/*Trong trường hợp là phép trừ*/ 
	else if (!strcmp(argv[1],"sub")) { 
		arg[0] = atoi(argv[2]); 
		arg[1] = atoi(argv[3]); 
		ioctl (fd, HELLOWORLD_SUB, arg); 
		printf ("User: %ld - %ld = %ld\n", arg[0], arg[1], 
		arg[2]); 
	} 
	/*Trong trường hợp là phép nhân*/ 
	else if (!strcmp(argv[1],"mul")) { 
		arg[0] = atoi(argv[2]); 
		arg[1] = atoi(argv[3]); 
		ioctl (fd, HELLOWORLD_MUL, arg); 
		printf ("User: %ld x %ld = %ld\n", arg[0], arg[1], 
		arg[2]); 
	} 
	/*Trong trường hợp là phép chia*/ 
	else if (!strcmp(argv[1],"div")) { 
		arg[0] = atoi(argv[2]); 
		arg[1] = atoi(argv[3]); 
		ioctl (fd, HELLOWORLD_DIV, arg); 
		printf ("User: %ld / %ld = %ld\n", arg[0], arg[1], 
		arg[2]); 
	} 
	/*Trong trường hợp không có lệnh hỗtrợ, in ra hướng dẫn cho người dùng*/ 
	else { 
		print_usage(); 
	} 
	} 
	/*Trong trường hợp là lệnh chép thông tin sang driver*/ 
	else if (argc == 3) { 
		if (!strcmp(argv[1],"write")) { 
			write_buf[0] = atoi(argv[2]); 
			printf ("User: has just sent number: %d\n", write_buf[0]); 
			write(fd, write_buf, 1); 
		} 
		else { 
			print_usage(); 
		} 
	} 
	/*Trong trường hợp lệnh đọc thông tin từ driver*/ 
	else if (argc == 2) { 
		if (!strcmp(argv[1],"read")) { 
			read(fd, read_buf, 1); 
			printf ("User: has just received number: %d\n", 
			read_buf[0]); 
			} 
		else { 
			print_usage(); 
		} 
	} 
	/*In ra hướng dẫn cho người dùng trong trường hợp không có lệnh nào hỗ trợ*/ 
	else { 
		print_usage(); 
	} 
} 
