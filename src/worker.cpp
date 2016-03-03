//
// Created by ilya on 03.03.16.
//

#include <iostream>
#include <thread>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>
#include "worker.h"
#include "http_parser.h"
#include "utils.h"

struct worker_data {
    int connection;
    const char *url_at;
    size_t url_length;
    worker *wrk;

    worker_data(int connection, worker *wrk) : connection(connection), url_at(nullptr), url_length(0), wrk(wrk) { }
};

int worker_url_cb(http_parser *parser, const char *at, size_t length);

int worker_notify_nothing_cb(http_parser *parser);

int worker_data_nothing_cb(http_parser *parser, const char *at, size_t length);

void worker::operator()() {
    auto tid = std::this_thread::get_id();
    const size_t buf_len = 80 * 1024;
    char buf[buf_len];
    ssize_t received;
    char fn[buf_len];
    size_t dir_length = strlen(dir);
    strncpy(fn, dir, dir_length);
    char *filename_ptr = fn + dir_length;
    char resp_200_begin[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
    size_t resp_200_begin_length = strlen(resp_200_begin);
    char resp_404[] = "HTTP/1.0 404 Not Found\r\n";
    size_t resp_404_length = strlen(resp_404);

    while (!queue.done) {
        int connection = 0;
        {
            std::unique_lock<std::mutex> lock(queue.m);
            while (!queue.notified) {
                queue.cond.wait(lock);
            }

            if (queue.done) {
                break;
            }

            connection = queue.storage.front();
            queue.storage.pop_front();
            queue.notified = false;
        }

        size_t rec_len = 0;

        while ((received = recv(connection, buf + rec_len, buf_len, 0)) > 0) {
            rec_len += received;
            fprintf(stderr, "%d %d\n", (int) rec_len, (int) received);

            if (received == 2 && buf[rec_len - 2] == '\r' && buf[rec_len - 1] == '\n') {
                break;
            }
        }

        if (received < 0) {
            close(connection);
            continue;
        }

        worker_data *data = new worker_data(connection, this);
        http_parser *parser = new http_parser;
        http_parser_init(parser, HTTP_REQUEST);
        parser->data = data;
        http_parser_settings settings;
        settings.on_url = worker_url_cb;
        settings.on_body = worker_data_nothing_cb;
        settings.on_header_field = worker_data_nothing_cb;
        settings.on_header_value = worker_data_nothing_cb;
        settings.on_status = worker_data_nothing_cb;
        settings.on_chunk_complete = worker_notify_nothing_cb;
        settings.on_chunk_header = worker_notify_nothing_cb;
        settings.on_message_begin = worker_notify_nothing_cb;
        settings.on_headers_complete = worker_notify_nothing_cb;
        settings.on_message_complete = worker_notify_nothing_cb;

        size_t parsed = http_parser_execute(parser, &settings, buf, (size_t) rec_len);

        if (!parser->upgrade && parsed == rec_len && data->url_at) {
            strncpy(filename_ptr, data->url_at, data->url_length);
            // TODO: replace ? sign
            // TODO: url decode
            filename_ptr[data->url_length] = '\0';
            struct stat st;
            int stat_res = stat(fn, &st);
            int fd;

            if (stat_res == 0 && ((st.st_mode & S_IFREG) || (st.st_mode & S_IFLNK)) && (fd = open(fn, O_RDONLY)) > 0) {
                write(connection, resp_200_begin, resp_200_begin_length);
                char len_str[20];
                sprintf(len_str, "%d\r\n\r\n", (int) st.st_size);
                write(connection, len_str, strlen(len_str));
                do_sendfile(connection, fd, 0, (size_t) st.st_size);
                close(fd);
            } else {
                write(connection, resp_404, resp_404_length);
            }
        }

        delete parser;
        delete data;
        close(connection);
    }

    std::cout << tid << " exit" << std::endl;
}

int worker_url_cb(http_parser *parser, const char *at, size_t length) {
    worker_data *data = (worker_data *) parser->data;
    data->url_at = at;
    data->url_length = length;

    return 0;
}

int worker_notify_nothing_cb(http_parser *parser) {
    return 0;
}

int worker_data_nothing_cb(http_parser *parser, const char *at, size_t length) {
    return 0;
}
