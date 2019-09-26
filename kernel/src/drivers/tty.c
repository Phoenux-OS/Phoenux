#include <stdint.h>
#include <kernel.h>

#define VIDEO_BOTTOM (VD_ROWS*VD_COLS)-1

void escape_parse(char c, uint8_t which_tty);

static char* video_mem = (char*)0xB8000;

static void clear_cursor(uint8_t which_tty) {
    tty[which_tty].field[(tty[which_tty].cursor_offset)+1] = tty[which_tty].text_palette;
    if (which_tty == current_tty)
        video_mem[(tty[which_tty].cursor_offset)+1] = tty[which_tty].text_palette;
    return;
}

static void draw_cursor(uint8_t which_tty) {
    if (tty[which_tty].cursor_status) {
        tty[which_tty].field[(tty[which_tty].cursor_offset)+1] = tty[which_tty].cursor_palette;
        if (which_tty == current_tty)
            video_mem[(tty[which_tty].cursor_offset)+1] = tty[which_tty].cursor_palette;
    }
    return;
}

static void scroll(uint8_t which_tty) {
    uint32_t i;
    // move the text up by one row
    for (i=0; i<=VIDEO_BOTTOM-VD_COLS; i++)
        tty[which_tty].field[i] = tty[which_tty].field[i+VD_COLS];
    // clear the last line of the screen
    for (i=VIDEO_BOTTOM; i>VIDEO_BOTTOM-VD_COLS; i -= 2) {
        tty[which_tty].field[i] = tty[which_tty].text_palette;
        tty[which_tty].field[i-1] = ' ';
    }
    tty_refresh(which_tty);
    return;
}

void text_clear(uint8_t which_tty) {
    uint32_t i;
    clear_cursor(which_tty);
    for (i=0; i<VIDEO_BOTTOM; i += 2) {
        tty[which_tty].field[i] = ' ';
        tty[which_tty].field[i+1] = tty[which_tty].text_palette;
    }
    tty[which_tty].cursor_offset = 0;
    draw_cursor(which_tty);
    tty_refresh(which_tty);
    return;
}

void text_clear_no_move(uint8_t which_tty) {
    uint32_t i;
    clear_cursor(which_tty);
    for (i=0; i<VIDEO_BOTTOM; i += 2) {
        tty[which_tty].field[i] = ' ';
        tty[which_tty].field[i+1] = tty[which_tty].text_palette;
    }
    draw_cursor(which_tty);
    tty_refresh(which_tty);
    return;
}

void text_enable_cursor(uint8_t which_tty) {
    tty[which_tty].cursor_status=1;
    draw_cursor(which_tty);
    return;
}

void text_disable_cursor(uint8_t which_tty) {
    tty[which_tty].cursor_status=0;
    clear_cursor(which_tty);
    return;
}

void text_putchar(char c, uint8_t which_tty) {
    if (tty[which_tty].escape) {
        escape_parse(c, which_tty);
        return;
    }
    switch (c) {
        case 0x00:
            break;
        case 0x1B:
            tty[which_tty].escape = 1;
            return;
        case 0x0A:
            if (text_get_cursor_pos_y(which_tty) == (VD_ROWS - 1)) {
                clear_cursor(which_tty);
                scroll(which_tty);
                text_set_cursor_pos(0, (VD_ROWS - 1), which_tty);
            } else
                text_set_cursor_pos(0, (text_get_cursor_pos_y(which_tty) + 1), which_tty);
            break;
        case 0x08:
            if (tty[which_tty].cursor_offset) {
                clear_cursor(which_tty);
                tty[which_tty].cursor_offset -= 2;
                tty[which_tty].field[tty[which_tty].cursor_offset] = ' ';
                if (which_tty == current_tty)
                    video_mem[tty[which_tty].cursor_offset] = ' ';
                draw_cursor(which_tty);
            }
            break;
        default:
            clear_cursor(which_tty);
            tty[which_tty].field[tty[which_tty].cursor_offset] = c;
            if (which_tty == current_tty)
                video_mem[tty[which_tty].cursor_offset] = c;
            if (tty[which_tty].cursor_offset >= (VIDEO_BOTTOM - 1)) {
                if (tty[which_tty].noscroll) goto dont_move;
                scroll(which_tty);
                tty[which_tty].cursor_offset = VIDEO_BOTTOM - (VD_COLS - 1);
            } else
                tty[which_tty].cursor_offset += 2;
dont_move:
            draw_cursor(which_tty);
    }
    return;
}

static uint8_t ansi_colours[] = { 0, 4, 2, 6, 1, 5, 3, 7 };

void sgr(uint8_t which_tty) {

    if (tty[which_tty].esc_value0 >= 30 && tty[which_tty].esc_value0 <= 37) {
        uint8_t pal = text_get_text_palette(which_tty);
        pal = (pal & 0xf0) + ansi_colours[tty[which_tty].esc_value0 - 30];
        text_set_text_palette(pal, which_tty);
        return;
    }

    if (tty[which_tty].esc_value0 >= 40 && tty[which_tty].esc_value0 <= 47) {
        uint8_t pal = text_get_text_palette(which_tty);
        pal = (pal & 0x0f) + ansi_colours[tty[which_tty].esc_value0 - 40] * 0x10;
        text_set_text_palette(pal, which_tty);
        return;
    }

    return;
}

void escape_parse(char c, uint8_t which_tty) {
    
    if (c >= '0' && c <= '9') {
        *tty[which_tty].esc_value *= 10;
        *tty[which_tty].esc_value += c - '0';
        *tty[which_tty].esc_default = 0;
        return;
    }

    switch (c) {
        case '[':
            return;
        case ';':
            tty[which_tty].esc_value = &tty[which_tty].esc_value1;
            tty[which_tty].esc_default = &tty[which_tty].esc_default1;
            return;
        case 'A':
            if (tty[which_tty].esc_default0) tty[which_tty].esc_value0 = 1;
            if (tty[which_tty].esc_value0 >
                text_get_cursor_pos_y(which_tty))
                tty[which_tty].esc_value0 = text_get_cursor_pos_y(which_tty);
            text_set_cursor_pos(text_get_cursor_pos_x(which_tty),
                                text_get_cursor_pos_y(which_tty)
                                - tty[which_tty].esc_value0,
                                which_tty);
            break;
        case 'B':
            if (tty[which_tty].esc_default0) tty[which_tty].esc_value0 = 1;
            if ((text_get_cursor_pos_y(which_tty) + tty[which_tty].esc_value0) >
                (VD_ROWS - 1))
                tty[which_tty].esc_value0 = (VD_ROWS - 1) - text_get_cursor_pos_y(which_tty);
            text_set_cursor_pos(text_get_cursor_pos_x(which_tty),
                                text_get_cursor_pos_y(which_tty)
                                + tty[which_tty].esc_value0,
                                which_tty);
            break;
        case 'C':
            if (tty[which_tty].esc_default0) tty[which_tty].esc_value0 = 1;
            if ((text_get_cursor_pos_x(which_tty) + tty[which_tty].esc_value0) >
                (VD_COLS / 2 - 1))
                tty[which_tty].esc_value0 = (VD_COLS / 2 - 1) - text_get_cursor_pos_x(which_tty);
            text_set_cursor_pos(text_get_cursor_pos_x(which_tty)
                                + tty[which_tty].esc_value0,
                                text_get_cursor_pos_y(which_tty),
                                which_tty);
            break;
        case 'D':
            if (tty[which_tty].esc_default0) tty[which_tty].esc_value0 = 1;
            if (tty[which_tty].esc_value0 >
                text_get_cursor_pos_x(which_tty))
                tty[which_tty].esc_value0 = text_get_cursor_pos_x(which_tty);
            text_set_cursor_pos(text_get_cursor_pos_x(which_tty)
                                - tty[which_tty].esc_value0,
                                text_get_cursor_pos_y(which_tty),
                                which_tty);
            break;
        case 'H':
            tty[which_tty].esc_value0 -= 1;
            tty[which_tty].esc_value1 -= 1;
            if (tty[which_tty].esc_default0) tty[which_tty].esc_value0 = 0;
            if (tty[which_tty].esc_default1) tty[which_tty].esc_value1 = 0;
            if (tty[which_tty].esc_value1 >= (VD_COLS / 2))
                tty[which_tty].esc_value1 = (VD_COLS / 2) - 1;
            if (tty[which_tty].esc_value0 >= VD_ROWS)
                tty[which_tty].esc_value0 = VD_ROWS - 1;
            text_set_cursor_pos(tty[which_tty].esc_value1, tty[which_tty].esc_value0, which_tty);
            break;
        case 'm':
            sgr(which_tty);
            break;
        case 'J':
            switch (tty[which_tty].esc_value0) {
                case 2:
                    text_clear_no_move(which_tty);
                    break;
                default:
                    break;
            }
            break;
        /* non-standard sequences */
        case 'r': /* enter/exit raw mode */
            tty[which_tty].raw = !tty[which_tty].raw;
            break;
        case 'b': /* enter/exit non-blocking mode */
            tty[which_tty].noblock = !tty[which_tty].noblock;
            break;
        case 's': /* enter/exit non-scrolling mode */
            tty[which_tty].noscroll = !tty[which_tty].noscroll;
            break;
        /* end non-standard sequences */
        default:
            tty[which_tty].escape = 0;
            text_putchar('?', which_tty);
            break;
    }
    
    tty[which_tty].esc_value = &tty[which_tty].esc_value0;
    tty[which_tty].esc_value0 = 0;
    tty[which_tty].esc_value1 = 0;
    tty[which_tty].esc_default = &tty[which_tty].esc_default0;
    tty[which_tty].esc_default0 = 1;
    tty[which_tty].esc_default1 = 1;
    tty[which_tty].escape = 0;

    return;
}

void text_set_cursor_palette(uint8_t c, uint8_t which_tty) {
    tty[which_tty].cursor_palette = c;
    draw_cursor(which_tty);
    return;
}

uint8_t text_get_cursor_palette(uint8_t which_tty) {
    return tty[which_tty].cursor_palette;
}

void text_set_text_palette(uint8_t c, uint8_t which_tty) {
    tty[which_tty].text_palette = c;
    return;
}

uint8_t text_get_text_palette(uint8_t which_tty) {
    return tty[which_tty].text_palette;
}

uint32_t text_get_cursor_pos_x(uint8_t which_tty) {
    return (tty[which_tty].cursor_offset % VD_COLS) / 2;
}

uint32_t text_get_cursor_pos_y(uint8_t which_tty) {
    return tty[which_tty].cursor_offset / VD_COLS;
}

void text_set_cursor_pos(uint32_t x, uint32_t y, uint8_t which_tty) {
    clear_cursor(which_tty);
    tty[which_tty].cursor_offset = (y*VD_COLS)+(x*2);
    draw_cursor(which_tty);
    return;
}

// -- tty --

tty_t tty[KRNL_TTY_COUNT];
uint8_t current_tty;

void switch_tty(uint8_t which_tty) {
    current_tty = which_tty;
    tty_refresh(which_tty);
    return;
}

void init_tty(void) {
    uint32_t i;
    for (i=0; i<KRNL_TTY_COUNT; i++) {
        tty[i].esc_value = &tty[i].esc_value0;
        tty[i].esc_value0 = 0;
        tty[i].esc_value1 = 0;
        tty[i].esc_default = &tty[i].esc_default0;
        tty[i].esc_default0 = 1;
        tty[i].esc_default1 = 1;
        tty[i].escape = 0;
        tty[i].cursor_offset = 0;
        tty[i].cursor_status = 1;
        tty[i].cursor_palette = TTY_DEF_CUR_PAL;
        tty[i].text_palette = TTY_DEF_TXT_PAL;
        tty[i].raw = 0;
        tty[i].noblock = 0;
        tty[i].noscroll = 0;
        for (uint32_t ii=0; ii<VIDEO_BOTTOM; ii += 2) {
            tty[i].field[ii] = ' ';
            tty[i].field[ii+1] = TTY_DEF_TXT_PAL;
        }
        tty[i].field[1] = TTY_DEF_CUR_PAL;
    }
    return;
}

void tty_refresh(uint8_t which_tty) {
    if (which_tty == current_tty)
        kmemcpy(video_mem, tty[current_tty].field, VD_ROWS*VD_COLS);
    return;
}
