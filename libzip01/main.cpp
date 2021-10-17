#include <cstdio>
#include <cstring>
#include <errno.h>
#include <string>
#include <zip.h>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace {

class ScopedZip {
  public:
    explicit ScopedZip(zip_t *z) : value_(z) {
    }

    ~ScopedZip() {
        zip_close(value_);
    }

    zip_t *value() const noexcept {
        return value_;
    }

  private:
    zip_t *value_;
};

} // namespace

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s zip file\n", argv[0]);
        return 1;
    }

    int zip_err = 0;
    zip_t *z = zip_open(argv[1], ZIP_CHECKCONS, &zip_err);
    if (z == nullptr) {
        char err_buf[256];
        zip_error_to_str(err_buf, 256, zip_err, errno);
        fprintf(stderr, "failed to open zip: %s\n", err_buf);
        return 1;
    }

    ScopedZip scoped_zip(z);

    zip_int64_t entries = zip_get_num_entries(z, 0);
    for (zip_int64_t i = 0; i < entries; ++i) {
        struct zip_stat st;
        if (zip_stat_index(z, i, 0, &st) != 0) {
            fprintf(stderr, "could not get file stat index: %" PRId64 "\n", i);
            continue;
        }

        size_t len = strlen(st.name);
        if (st.name[len - 1] == '/') {
            printf("## create directory %s\n", st.name);
            if (!std::filesystem::create_directories(st.name)) {
                fprintf(stderr, "failed to create %s directory\n", st.name);
                continue;
            }
        } else {
            printf("## extract %s\n", st.name);

            zip_file_t *zf = zip_fopen_index(z, i, 0);
            if (zf == nullptr) {
                fprintf(stderr, "failed to open %s\n", st.name);
                return 1;
            }

            int fd = open(st.name, O_RDWR | O_TRUNC | O_CREAT, 0644);
            if (fd == -1) {
                perror("open");
                return 1;
            }

            zip_uint64_t total = 0;
            char buf[4096];
            while (total != st.size) {
                zip_int64_t len = zip_fread(zf, buf, 4096);
                if (len < 0) {
                    fprintf(stderr, "failed to read content of %s\n", st.name);
                    return 1;
                }

                (void)write(fd, buf, len);
                total += len;
            }

            close(fd);
            zip_fclose(zf);
        }
    }

    return 0;
}
