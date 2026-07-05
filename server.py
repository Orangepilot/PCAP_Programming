import socket

#TCP 소켓 생성
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#8080포트에서 대기
server_socket.bind(('0.0.0.0', 8080))
server_socket.listen(3)

print("서버 대기중 ...(port 8080)")

#클라이언트의 접속 받기
client_socket, addr = server_socket.accept()
print(f"클라이언트 접속됨: {addr}\n")

# 4. 무한 루프로 실시간 채팅 시작
while True:
    # [수신] 클라이언트의 메시지 기다리기
    data = client_socket.recv(1024).decode('utf-8')
    
    # 상대방이 exit를 보냈거나 연결이 끊기면 루프 탈출
    if not data or data.lower() == 'exit':
        print("\n클라이언트가 통신을 종료했습니다.")
        break
        
    print(f"클라이언트: {data}")

    # (송신)서버에서 메시지 직접 입력하여 보내기
    message = input("서버: ")
    client_socket.send(message.encode('utf-8'))
    
    # exit를 치면 루프 탈출
    if message.lower() == 'exit':
        print("\n통신을 종료합니다.")
        break

# 5. 통신 종료
client_socket.close()
server_socket.close()