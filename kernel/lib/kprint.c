#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <lib/kprint.h>


#include <drv/vga_textmode.h>

static void kprint_buf_flush(char *kprint_buf, size_t *kprint_buf_i) {
    for (size_t i = 0; i < *kprint_buf_i; i++)
        text_putchar(kprint_buf[i]);
}

static void kprint_buf_flush_urgent(char *kprint_buf, size_t *kprint_buf_i) {
    for (size_t i = 0; i < *kprint_buf_i; i++)
        text_putchar(kprint_buf[i]);
}


#define KPRINT_BUF_MAX 256

static void kputs(char *kprint_buf, size_t *kprint_buf_i, const char *string) {
    size_t i;

    for (i = 0; string[i]; i++) {
        if (*kprint_buf_i == (KPRINT_BUF_MAX - 1))
            break;
        kprint_buf[(*kprint_buf_i)++] = string[i];
    }

    kprint_buf[*kprint_buf_i] = 0;

    return;
}

static void knputs(char *kprint_buf, size_t *kprint_buf_i, const char *string, size_t len) {
    size_t i;

    for (i = 0; i < len; i++) {
        if (*kprint_buf_i == (KPRINT_BUF_MAX - 1))
            break;
        kprint_buf[(*kprint_buf_i)++] = string[i];
    }

    kprint_buf[*kprint_buf_i] = 0;

    return;
}

static void kputchar(char *kprint_buf, size_t *kprint_buf_i, char c) {
    if (*kprint_buf_i < (KPRINT_BUF_MAX - 1)) {
        kprint_buf[(*kprint_buf_i)++] = c;
    }

    kprint_buf[*kprint_buf_i] = 0;

    return;
}

static void kprn_i(char *kprint_buf, size_t *kprint_buf_i, int64_t x) {
    int i;
    char buf[21] = {0};

    if (!x) {
        kputchar(kprint_buf, kprint_buf_i, '0');
        return;
    }

    int sign = x < 0;
    if (sign) x = -x;

    for (i = 19; x; i--) {
        buf[i] = (x % 10) + 0x30;
        x = x / 10;
    }
    if (sign)
        buf[i] = '-';
    else
        i++;

    kputs(kprint_buf, kprint_buf_i, buf + i);

    return;
}

static void kprn_ui(char *kprint_buf, size_t *kprint_buf_i, uint64_t x) {
    int i;
    char buf[21] = {0};

    if (!x) {
        kputchar(kprint_buf, kprint_buf_i, '0');
        return;
    }

    for (i = 19; x; i--) {
        buf[i] = (x % 10) + 0x30;
        x = x / 10;
    }

    i++;
    kputs(kprint_buf, kprint_buf_i, buf + i);

    return;
}

static const char *hex_to_ascii_tab = "0123456789abcdef";

static void kprn_x(char *kprint_buf, size_t *kprint_buf_i, uint64_t x) {
    int i;
    char buf[17] = {0};

    if (!x) {
        kputs(kprint_buf, kprint_buf_i, "0x0");
        return;
    }

    for (i = 15; x; i--) {
        buf[i] = hex_to_ascii_tab[(x % 16)];
        x = x / 16;
    }

    i++;
    kputs(kprint_buf, kprint_buf_i, "0x");
    kputs(kprint_buf, kprint_buf_i, buf + i);

    return;
}

static void print_header(char *kprint_buf, size_t *kprint_buf_i, int type) {
    switch (type) {
        case KPRN_INFO:
            kputs(kprint_buf, kprint_buf_i, "\e[36minfo\e[37m: ");
            break;
        case KPRN_WARN:
            kputs(kprint_buf, kprint_buf_i, "\e[33mwarning\e[37m: ");
            break;
        case KPRN_ERR:
            kputs(kprint_buf, kprint_buf_i, "\e[31mERROR\e[37m: ");
            break;
        case KPRN_PANIC:
            kputs(kprint_buf, kprint_buf_i, "\e[31mPANIC\e[37m: ");
            break;
        default:
        case KPRN_DBG:
            kputs(kprint_buf, kprint_buf_i, "\e[36mDEBUG\e[37m: ");
            break;
    }
}

void kprint(int type, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    kvprint(type, fmt, args);
    va_end(args);

    return;
}

void kvprint(int type, const char *fmt, va_list args) {
    char kprint_buf[KPRINT_BUF_MAX];
    size_t kprint_buf_i = 0;

    print_header(kprint_buf, &kprint_buf_i, type);

    char *str;
    size_t str_len;

    for (;;) {
        char c;

        while (*fmt && *fmt != '%') {
            kputchar(kprint_buf, &kprint_buf_i, *fmt);
            if (*fmt == '\n')
                print_header(kprint_buf, &kprint_buf_i, type);
            fmt++;
        }
        if (!*fmt++) {
            kputchar(kprint_buf, &kprint_buf_i, '\n');
            goto out;
        }
        switch (*fmt++) {
            case 's':
                str = (char *)va_arg(args, const char *);
                if (!str)
                    kputs(kprint_buf, &kprint_buf_i, "(null)");
                else
                    kputs(kprint_buf, &kprint_buf_i, str);
                break;
            case 'S':
                str_len = va_arg(args, size_t);
                str = (char *)va_arg(args, const char *);
                knputs(kprint_buf, &kprint_buf_i, str, str_len);
                break;
            case 'd':
                kprn_i(kprint_buf, &kprint_buf_i, (int64_t)va_arg(args, int));
                break;
            case 'D':
                kprn_i(kprint_buf, &kprint_buf_i, (int64_t)va_arg(args, int64_t));
                break;
            case 'u':
                kprn_ui(kprint_buf, &kprint_buf_i, (uint64_t)va_arg(args, unsigned int));
                break;
            case 'U':
                kprn_ui(kprint_buf, &kprint_buf_i, (uint64_t)va_arg(args, uint64_t));
                break;
            case 'x':
                kprn_x(kprint_buf, &kprint_buf_i, (uint64_t)va_arg(args, unsigned int));
                break;
            case 'X':
                kprn_x(kprint_buf, &kprint_buf_i, (uint64_t)va_arg(args, uint64_t));
                break;
            case 'c':
                c = (char)va_arg(args, int);
                kputchar(kprint_buf, &kprint_buf_i, c);
                break;
            default:
                kputchar(kprint_buf, &kprint_buf_i, '?');
                break;
        }
    }

out:
    if (type == KPRN_INFO || type == KPRN_DBG) {
        kprint_buf_flush(kprint_buf, &kprint_buf_i);
    } else {
        kprint_buf_flush_urgent(kprint_buf, &kprint_buf_i);
    }

    return;
}
