## 3. Functional Specification

<!-- TODO ### 20240829 | 3.1. Header Format -->
<!-- TODO ### 20240829 | 3.2. Specific Option Definitions -->
<!-- TODO ### 20240829 | 3.3. TCP Terminology Overview -->
<!-- TODO ### 20240829 | 3.4. Sequence Numbers  -->

### 3.5. Establishing a Connection

The __Three-way Handshake__ is the procedure used to establish a connection. This procedure normally is initiated by one TCP peer and responded by another TCP peer. The procedure also works if tow TCP peers simultaneously initiate the procedure. When simultaneous open occurs, each TCP peer receives a SYN segment that carries no acknowledgment after it has sent a SYN. Of course, the arrival of an old duplicate SYN segment can potentially make it appear, to the recipient, that a simultaneous connection initiation is in progress. Proper use of "reset" segments can disambiguate these cases.

<!--

Three-way Handshake는 연결을 설정하는 데 사용되는 절차입니다. 이 절차는 일반적으로 하나의 TCP 피어에 의해 시작되고 다른 TCP 피어에 의해 응답됩니다. 이 절차는 견인 TCP 피어가 동시에 절차를 시작하는 경우에도 작동합니다. 동시 오픈이 발생하면 각 TCP 피어는 SYN을 보낸 후에 아무런 확인 응답을 전송하지 않는 SYN 세그먼트를 받습니다. 물론 오래된 중복 SYN 세그먼트가 도착하면 잠재적으로 수신자에게 동시 연결 시작이 진행 중인 것처럼 보일 수 있습니다. "reset" 세그먼트를 적절하게 사용하면 이러한 경우를 명확하게 할 수 있습니다.

  -->

Several examples of connection initiation follow. Although these examples do not show connection synchronization using data-carrying segments, this perfectly legitimate, so long as the receiving TCP endpoint doesn't deliver the data to the user until it is clear the data is valid (e.g., the data is buffered at the receiver until the connection reaches the established state, given that the Three-way Handshake reduces the possibility of false connections). It is a trade-off between memory and messages to provide information for this checking.

<!--

연결 시작의 몇 가지 예는 다음과 같습니다. 이러한 예들은 데이터 전송 세그먼트를 사용한 연결 동기화를 보여주지는 않지만, 수신 TCP 엔드포인트가 데이터가 유효하다는 것이 명확할 때까지 데이터를 사용자에게 전달하지 않는 한, 이것은 완벽하게 합법적입니다(예: 삼자 핸드셰이크가 잘못된 연결의 가능성을 줄여준다는 점을 감안할 때, 연결이 설정된 상태에 도달할 때까지 수신기에서 데이터를 버퍼링합니다). 이 확인을 위한 정보를 제공하는 것은 메모리와 메시지 사이의 트레이드오프입니다.

  -->

This simplest Three-way Handshake <sup>3WHS</sup> is shown in Figure 6. The figures should be interpreted in the following way. Each line is numbered for reference purposes. Right arrow `-->` indicate departure of a TCP segment from TCP Peer A to TCP Peer B or arrival of a segment at B from A. Left arrows `<--` indicate the reverse. Ellipses `...` indicate a segment that is still in the network (delayed). Comments appear in parentheses. TCP connection states represent the state after the departure or arrival of the segment (whose contents are shown in the center of each line). Segment contents are shown in abbreviated form, with sequence number, control flags, and ACK field. Other fields such as window, addresses, lengths, and text have been left out in the interest of clarity.

<!--

이 가장 간단한 3방향 핸드셰이크 3WHS는 그림 6에 나와 있습니다. 그림은 다음과 같은 방식으로 해석되어야 합니다. 각 행에는 참조용으로 번호가 매겨집니다. 오른쪽 화살표 --\> 는 TCP 피어 A에서 TCP 피어 B로 TCP 세그먼트의 출발 또는 A에서 B로 세그먼트의 도착을 나타냅니다. 왼쪽 화살표 \<-- 는 그 반대를 나타냅니다. 타원...은 아직 네트워크에 있는 세그먼트(지연됨)를 나타냅니다. 주석은 괄호 안에 표시됩니다. TCP 연결 상태는 세그먼트의 출발 또는 도착 이후의 상태(각 행의 중앙에 내용이 표시됨)를 나타냅니다. 세그먼트 내용은 시퀀스 번호, 제어 플래그 및 ACK 필드와 함께 축약된 형태로 표시됩니다. 창, 주소, 길이 및 텍스트와 같은 다른 필드는 명확성을 위해 생략되었습니다.

  -->

![Figure 6: Basic Three-way Handshake for Connection Synchronization](./images/Figure6.Basic-Three-way-Handshake-for-Connection-Synchronization.png)

Figure 6: Basic Three-way Handshake for Connection Synchronization

In line 2 of Figure 6, TCP Peer A begins by sending a SYN segment indicating that it will use sequence number 100. In line 3, TCP Peer B sends a SYN and acknowledges the SYN it received from TCP Peer A. Note that the acknowledgment field indicates TCP Peer B is now expecting to hear sequence 101, acknowledging the SYN that occupied sequence 100.

At line 4, TCP Peer A responds with an empty segment containing an ACK for TCP Peer B's SYN; and in line 5, TCP Peer A sends some data. Note that the sequence number of the segment in line 5 is the same as in line 4 because the ACK does not occupy sequence number space (if it did, we would wind up ACK).

Simultaneous initiation is only slightly more complex, as is shown in Figure 7. Each TCP peer's connection state cycles from CLOSED to SYN-SENT to SYN-RECEIVED to ESTABLISHED.

![Figure 7: Simultaneous Connection Synchronization](./images/Figure7.Simultaneous-Connection-Synchronization.png)

A TCP implementation must support simultaneous open attempts. <sup>MUST10</sup>

Note that a TCP implementation must keep track of whether a connection has reached SYN-RECEIVED state as the result of a passive open or an active open. <sup>MUST11</sup>

The principal reason for the Three-way Handshake is to prevent old duplicate connection initiations from causing confusion. To deal with this, a special control message, reset, is specified. If the receiving TCP peer is in a non-synchronized state (i.e., SYN-SENT, SYN-RECEIVED), it returns to LISTEN on receiving TCP peer is in a non-synchronized state (i.e., SYN-SENT, SYN-RECEIVED), it returns to LISTEN on receiving an acceptable reset. If the TCP peer is one of the synchronized state (ESTABLISHED, FIN-WAIT-1, FIN-WAIT-2, CLOSE-WAIT, CLOSING, LAST-ACK, TIME-WAIT), it aborts the connection and informs its user. We discuss this latter case under "half-open" connections below.

![Figure 8: Recovery from Old Duplicate SYN](./images/Figure8.Recovery-from-Old-Duplicate-SYN.png)

As a simple example of recovery from old duplicates, consider Figure 8. At line 3, an old duplicate SYN arrives at TCP Peer B. TCP Peer B cannot tell that this is an old duplicate, so it responds normally (line 4). TCP Peer A detects that the ACK field is incorrect and returns a RST (reset) with its SEQ field selected to make the segment believable. TCP Peer B, on receiving the RST, returns to the LISTEN state. When the original SYN finally arrives at line 6, the synchronization proceeds normally. If the SYN at line 6 had arrived before the RST, a more complex exchange might have occurred with RST sent in both directions.

#### 3.5.1. Half-Open Connections and Other Anomalies

An established connection is said to be "half-open" if one of the TCP peers has closed or aborted the connection at its end without the knowledge of the other, or if the two ends of the connection have become desynchronized owing to a failure or reboot that resulted in loss of memory. Such connections will automatically become reset if an attempt is made to send data in either direction. However, half-open connections are expected to be unusual.

If at site A the connection no longer exists, then an attempt by the user at site B to send any data on it will result in the site B TCP endpoint receiving a reset control message. Such a message indicates to the site B TCP endpoint that something is wrong, and it is expected to abort the connection.

Assume that two user processes A and B are communicating with one another when a failure or reboot occurs causing loss of memory to A TCP implementation. Depending on the operating system supporting A TCP implementation, it is likely that some error recovery mechanism exists. When the TCP endpoint is up again, A is likely to start again from the beginning or from a recovery point. As a result, A will probably try to open the connection again or try to send on the connection it believes open. In the latter case, it receives the error message "connection not open" from the local TCP implementation. In an attempt establish the connection, A TCP implementation will send a segment containing SYN. This senario leads to the example shown in Figure 9. After TCP Peer A reboots, the user attempts to reopen the connection. TCP Peer B, in the meantime, thinks the connection is open.

![Figure 9: Half-Open Connection Discovery](./images/Figure9.Half-Open-Connection-Discovery.png)

Figure 9: Half-Open Connection Discovery

When the SYN arrives at line 3, TCP Peer B, being in a synchronized state, and the incoming segment outside the window, responds with an acknowledgment indicating what sequence it next expects to hear. TCP Peer A sees that this segment does not acknowledge anything it sent and, being unsynchronized, sends a reset<sup>RST</sup> because it has detected a half-open connection. TCP Peer B aborts at line 5. TCP Peer A will continue to try to establish the connection; the problem is now reduced to the basic Three-way Handshake of Figure 6.

An interesting alternative case occurs when TCP Peer A reboots and TCP Peer B tries to send data on what it thinks is a synchronized connection. This is illustrated in Figure 10. In this case, the data arriving at TCP Peer A from TCP Peer B (line 2) is unacceptable because no such connection exists, so TCP Peer A sends a RST. The RST is acceptable so TCP Peer B processes it and aborts the connection.

![Active Side Causes Half-Open Connection Discovery](./images/Figure10.Active-Side-Causes-Half-Open-Connection-Discovery.png)

In Figure 11, two TCP Peer A and B with passive connections waiting for SYN are depcited. An old duplicate arriving at TCP Peer B (line 2) stirs B into action. A SYN-ACK is returned (line 3) and causes TCP A to generate a RST (the ACK in line 3 is not acceptable). TCP Peer B accepts the reset and returns to its passive LISTEN state.

![Figure 11: Old Duplicate SYN Initiates a Reset on Two Passive Sockets](./images/Figure11.Old-Duplicate-SYN-Initiates-a-Reset-on-Two-Passive-Sockets.png)

A variety of other cases possible, all of which are accounted for by the following rules for RST generation and processing.

#### 3.5.2. Reset Generation

A TCP user or application can issue a reset on a connection at any time, though reset events are also generated by the protocol itself when various error conditions occurs, as described below. The side of connection issuing a reset should enter the TIME-WAIT state, as this generally helps to reduce the load on busy servers for reasons described in [The TIME-WAIT state in TCP and its effect on busy servers](https://ieeexplore.ieee.org/document/752180).

As a general rule, reset <sup>RST</sup> is sent whenever a segment arrives that apparently is not intended for the current connection. A reset must not be sent if it is not clear that this is the case.

There are three groups of states:

1. If the connection does not exist (CLOSED), then a reset is sent in response to any incoming segment except another reset. A SYN segment that does not match an existing connection is rejected by this means.

   If the incoming segment has the ACK bit set, the reset takes its sequence number from the ACK field of the segment; otherwise, the reset has sequence number zero and the ACK field is set to the sum of sequence number and segment length of the incoming segment. The connection remains in the CLOSED state.

2. If the connection is in any non-synchronized state (LISTEN, SYN-SENT, SYN-RECEIVED), and the incoming segment acknowledges something not yet sent (the segment carries an unacceptable ACK), or if an incoming segment has a security level or compartment <sup>[Appendix A.1](https://www.ietf.org/rfc/rfc9293.html#seccomp)</sup> that does not exactly matach the level and compartment requested for the connection, a reset is sent.

   If the incoming segment has an ACK field, the reset takes its sequence number from the ACK field of the segment; otherwise, the reset has sequence number zero and the ACK field is set to the sum of the sequence number and segment length of the incoming segment. The connection remains in the same state.

3. If the connection is in a synchronized state (ESTABLISHED, FIN-WAIT-1, FIN-WAIT-2 CLOSE-WAIT, CLOSING, LAST-ACK, TIME-WAIT), any unacceptable segment (out-of-window sequence number of unacceptable acknowledgment number) must be responded to with an empty acknowledgment segment (without any user data) containing the current send sequence number and an acknowledgment indicating the next sequence number expected to be received, and the connection remains in the same state.

   If an incoming segment has a security level or compartment that does not exactly match the level and compartment requested for the connection, a reset is sent and the connection goes to the CLOSED state. The reset takes its sequence number from the ACK field of the incoming segment.

#### 3.5.3. Reset Processing

In all states except SYN-SENT, all reset <sup>RST</sup> segments are validated by checking their SEQ fields. A reset is valid if its sequence number is in the window. In the SYN-SENT state (a RST received in response to an inital SYN), the RST is acceptable if the ACK field acknowledges the SYN.

The receiver of a RST first validates it, then change state. If the receiver was in the LISTEN state, it ignores it. If the receiver was in SYN-RECEIVED state and had previously been in the LISTEN state, then the receiver returns to the LISTEN state; otherwise, the receiver aborts the connection and does to the CLOSED state. If the receiver was in any other state, it aborts the connection and advises the user and goes to the CLOSED state

TCP implementations should allow a received RST segment to include data <sup>SHOULD 2</sup>. It has been suggested that a RST segment could contain diagnostic data that explains the cause of the RST. No standard has yet been established for such data.

