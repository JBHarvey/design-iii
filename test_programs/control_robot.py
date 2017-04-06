#!/usr/bin/env python
import sys
import termios
import contextlib
import socket
import struct

@contextlib.contextmanager
def raw_mode(file):
    old_attrs = termios.tcgetattr(file.fileno())
    new_attrs = old_attrs[:]
    new_attrs[3] = new_attrs[3] & ~(termios.ECHO | termios.ICANON)
    new_attrs[6][termios.VMIN] = 0  # cc
    new_attrs[6][termios.VTIME] = 0 # cc
    try:
        termios.tcsetattr(file.fileno(), termios.TCSADRAIN, new_attrs)
        yield
    finally:
        termios.tcsetattr(file.fileno(), termios.TCSADRAIN, old_attrs)

def send_packet(sock, packet):
    sock.send(struct.pack("<I", len(packet)) + packet)

def send_direction(sock, x, y):
    packet = struct.pack("BB", 17, 8) + struct.pack("ff", x, y)
    send_packet(sock, packet)

def send_rotate(sock, angle):
    packet = struct.pack("BB", 17, 1) + struct.pack("f", angle)
    send_packet(sock, packet)

def send_manchester(sock):
    packet = struct.pack("BB", 17, 6)
    send_packet(sock, packet)

def red_led(sock):
    packet = struct.pack("BB", 17, 2)
    send_packet(sock, packet)

def green_led(sock):
    packet = struct.pack("BB", 17, 3)
    send_packet(sock, packet)

def pencil_down(sock):
    packet = struct.pack("BB", 17, 5)
    send_packet(sock, packet)

def pencil_up(sock):
    packet = struct.pack("BB", 17, 4)
    send_packet(sock, packet)

def get_image(sock):
    packet = struct.pack("B", 18)
    send_packet(sock, packet)

def main():
    print 'exit with ^C or ^D'
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("10.42.0.1", 35794))
    #sock.connect(("127.0.0.1", 35794))
    sock.setblocking(0)

    with raw_mode(sys.stdin):
        try:
            while True:
                try:
                    data = sock.recv(1024)
                    if (len(data) > 0):
                        print "data:" + data
                except socket.error:
                    pass

                ch = sys.stdin.read(1)
                if not ch or ch == chr(4):
                    continue

                if ch == chr(0x41):
                    send_direction(sock, 0.1, 0.0)
                    print "arrow up"
                if ch == chr(0x42):
                    print "arrow down"
                    send_direction(sock, -0.1, 0.0)
                if ch == chr(0x43):
                    send_direction(sock, 0.0, -0.1)
                    print "arrow right"
                if ch == chr(0x44):
                    send_direction(sock, 0.0, 0.1)
                    print "arrow left"

                if ch == chr(0x61): #a
                    send_direction(sock, 0.0, 0.0)

                if ch == chr(0x6f): #o
                    send_rotate(sock, 0.0)

                if ch == chr(0x6d): #m
                    send_manchester(sock)

                if ch == chr(0x31): #1
                    red_led(sock)

                if ch == chr(0x32): #2
                    green_led(sock)

                if ch == chr(0x3b): #;
                    pencil_down(sock)

                if ch == chr(0x71): #q
                    pencil_up(sock)

                if ch == chr(0x35):
                    print "page up"
                    send_rotate(sock, 1.0)

                if ch == chr(0x36):
                    print "page down"
                    send_rotate(sock, -1.0)

                if ch == chr(0x20):
                    print "space"
                    get_image(sock)


                print '%02x' % ord(ch),
        except (KeyboardInterrupt, EOFError):
            pass


if __name__ == '__main__':
    main()
