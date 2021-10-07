#include <cstdio>
#include <errno.h>
#include <string>
#include <zip.h>

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
        fprintf(stderr, "Usage: %s zip file", argv[0]);
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

        printf("name: %s, size=%" PRIu64 ", comp_size=%" PRIu64 "\n", st.name, st.size, st.comp_size);
    }

    return 0;
}
