import socket
import sys

def consume_swo():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect(('127.0.0.1', 22888))
        print("Connected to OpenOCD SWO port. Waiting for data...\n")
    except Exception as e:
        print(f"Connection failed: {e}")
        return

    while True:
        data = s.recv(1024)
        if not data:
            break
        
        for byte in data:
            # ITM Header для Port 0 зазвичай має вигляд 0x01, 0x02 або 0x03 (залежно від розміру даних)
            # Якщо це не заголовок порту, виводимо як символ
            if byte >= 32 or byte == 10 or byte == 13: # ASCII текст, LF (\n), CR (\r)
                sys.stdout.write(chr(byte))
        sys.stdout.flush()

if __name__ == "__main__":
    consume_swo()