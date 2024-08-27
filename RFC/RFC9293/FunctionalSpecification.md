## 3. FunctionalSpecification



### 3.3. TCP Terminology Overview

This section includes an overview of key terms needed to understand the detailed protocol operation in the rest of the document. There is a glossary of terms in [Section 4. Glossary](./Glossary.md).

#### 3.3.1. Key Connection State Variables

Before we can discuss the operation of the Transmission Control Protocol <sup>TCP</sup> implementation is detail, we need to introduce some detailed terminology. The maintenance of a Transmission Control Protocol <sup>TCP</sup> connection requires maintaining state for several variables. We conceive of these variables being stored in a connection record called a Transmission Control Block <sup>TCB</sup>. Among the variables stored in the Transmission Control Block <sup>TCB</sup> are the local and remote Internet Protocol <sup>IP</sup> addresses and port numbers, the Internet Protocol <sup>IP</sup> security level, and compartment of the connection (see [Appendix A.1](./AppendixA.md#1-IP-Security-Compartment-and-Precedence)), pointers to the user's send and receive buffers, pointers to the retransmit queue and to the current segment. In addition, several variables relating to the send and receive sequence numbers are stored in the Transmission Control Block <sup>TCB</sup>.

<!--

The maintenance of a Transmission Control Protocol <sup>TCP</sup> connection requires maintaining state for several variables.

We conceive of these variables being stored in a connection record called a Transmission Control Block <sup>TCB</sup>.

Among the variables stored in the Transmission Control Block <sup>TCB</sup> are the local and remote Internet Protocol <sup>IP</sup> addresses and port numbers, the Internet Protocol <sup>IP</sup> addresses and port numbers, the Internet Protocol <sup>IP</sup> security level, and compartment of the connection, pointers to the user's send and receive buffers, pointers ot the retransmit queue nd to the current segment. In addition, several variables relating to the send and receive sequence numbers are stored in the Transmission Control Block <sup>TCB</sup>.

  -->

| Variable | Description                                               |
| -------- | --------------------------------------------------------- |
| SND.UNA  | Send Unacknowledged                                       |
| SND.NXT  | Send Next                                                 |
| SND.WND  | Send Window                                               |
| SND.UP   | Send Urgent Pointer                                       |
| SND.WL1  | Segment sequence number used for last window update       |
| SND.WL2  | Segment acknowledgment number used for last window update |
| ISS      | Initial Send Sequence number                              |

Table 2: Send Sequence Variables

<!-- Summary: Table 2: Send Sequence Variables -->

| Variable | Description                     |
| -------- | ------------------------------- |
| RCV.NXT  | Receive Next                    |
| RCV.WND  | Receive Window                  |
| RCV.UP   | Receive Urgent Pointer          |
| IRS      | Initial Receive Sequence number |

Table 3: Receive Sequence Variables

<!-- Summary: Table 3: Receive Sequence Variables -->

The following diagrams may help to relate some of these variables to the sequence space.

![Figure 3. Send Sequence Space](./images/Figure3.SendSequenceSpace.png)

- 1 - Old sequence numbers that have been acknowledged
- 2 - Sequence numbers of unacknowledged data
- 3 - Sequence numbers allowed for new data transmission
- 4 - Future sequence numbers that are not yet allowed

Figure 3: Send Sequence Space

The send window is the portion of the sequence space labeled 3 in Figure 3.

<!-- Figure 3: Send Sequence Space -->

![Figure 4: Receive Sequence Space](./images/Figure4.ReceiveSequenceSpace.png)

- 1 - Old sequence numbers that have been acknowledged
- 2 - Sequence numbers allowed for new reception
- 3 - Future sequence numbers that are not yet allowed

Figure 4: Receive Sequence Space

<!-- Figure 4: Receive Sequence Space -->

The receive window is the portion of the sequence space labeled 2 in Figure 4.

There are also some variables used frequently in the discussion that take their values from the fields of the current segment.

| Variable | Description                   |
| -------- | ----------------------------- |
| SEG.SEQ  | Segment Sequence number       |
| SEG.ACK  | Segment Acknowledgment number |
| SEG.LEN  | Segment Length                |
| SEG.WND  | Segment Window                |
| SEG.UP   | Segment Urgent Pointer        |

Table 4: Current Segment Variables

<!-- Table 4: Current Segment Variables -->

#### 3.3.2. State Machine Overview

A connection progresses through a series of states during its lifetime. The states are: LISTEN, SYN-SENT, SYN-RECEIVED, ESTABLISHED, FIN-WAIT-1, FIN-WAIT-2, CLOSE-WAIT, CLOSING, LAST-ACK, TIME-WAIT, and the fictional state CLOSED. CLOSED is fictional because it represents the state when there is no Transmission Control Block 
<sup>TCB</sup>, and therefore, no connection. Briefly the meanings of the states are:

<!-- TODO -->

#### 3.8.6. Managing the Window












