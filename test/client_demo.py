#! /usr/bin/python
# coding: utf-8

import socket
import time
import random

def main():

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 23333))

    while True:
        try:
            content = raw_input()
            if (content == 'exit'):
                break
            sock.send(content)
            print(sock.recv(1024))
        except KeyboardInterrupt:
            break

    sock.close()

    return 0

if __name__ == '__main__':
    STATUS = main()
