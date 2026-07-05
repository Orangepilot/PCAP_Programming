import socket

#TCP 소켓 생성
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#서버(내 컴퓨터의 8080 포트)로 접속
client_socket.connect(('127.0.0.1', 8080))
print("서버에 연결되었습니다 (종료하려면 'exit' 입력)\n")

#무한 루프로 실시간 채팅 시작
while True:
    #(송신)클라이언트에서 메시지 직접 입력하여 보내기
    message = input("클라이언트: ")
    client_socket.send(message.encode('utf-8'))
    
    # 내가 exit를 치면 루프 탈출
    if message.lower() == 'exit':
        print("\n통신을 종료합니다.")
        break

    #(수신)서버의 응답 기다리기
    data = client_socket.recv(1024).decode('utf-8')
    
    # 상대방이 exit를 보냈거나 연결이 끊기면 루프 탈출
    if not data or data.lower() == 'exit':
        print("\n서버가 통신을 종료했습니다.")
        break
        
    print(f"서버: {data}")

# 4. 통신 종료
client_socket.close()