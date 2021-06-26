import sys
import tornado
import tornado.httpserver
import tornado.httputil
import tornado.ioloop
import tornado.netutil
import tornado.process
import tornado.web
import socket


class KyuHandler(tornado.web.RequestHandler):
    SUPPORTED_METHODS = ["GET", "HEAD", "POST", "PUT", "DELETE", "OPTIONS"]

    async def _work(self, mode):
        # relay response to remote server
        print(self._headers)
        # write response
        self.set_status(200, "OK")
        self._headers = tornado.httputil.HTTPHeaders()
        self.add_header("Cache-Control", "no-store")
        self.add_header("Connection", "close")
        self.set_header("Content-Type", "application/json")
        self.add_header("Content-Length", str(len(response)))
        self.add_header(
            "Access-Control-Allow-Origin", self.request.headers.get("Origin", "*")
        )
        # push result to client
        self.write(response)
        self.flush()
        self.finish()

    pass


def main(argv):
    # prepare server
    # create web application
    app = tornado.web.Application(
        [(r".*", KyuHandler)],
        xsrf_cookies=False,
        compress_response=False,
    )
    # bind sockets with ipv4 / ipv6 support
    sock = tornado.netutil.bind_sockets(config.server_port, family=socket.AF_UNSPEC)
    server = tornado.httpserver.HTTPServer(
        app, max_body_size=64 * 1024 * 1024, xheaders=True
    )
    server.add_sockets(sock)
    # this allows SIGINT to be caught specifically on Windows
    def sleep_func(func, loop):
        loop.call_later(0.1, func, func, loop)

    tornado_loop = tornado.ioloop.IOLoop.current()
    loop = tornado_loop.asyncio_loop
    loop.call_later(0.0, sleep_func, sleep_func, loop)
    # start ioloop
    tornado_loop.start()
    return


if __name__ == "__main__":
    main(sys.argv)
