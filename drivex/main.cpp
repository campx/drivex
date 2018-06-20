#define FUSE_USE_VERSION 26
int main(int argc, char* argv[])
{
    struct fuse_operations hello_oper = {};
    hello_oper.getattr = hello_getattr;
    hello_oper.readdir = hello_readdir;
    hello_oper.open = hello_open;
    hello_oper.read = hello_read;
    auto fuse = Fuse{};
    return fuse_main(argc, argv, &hello_oper, (void*)(&fuse));
}
