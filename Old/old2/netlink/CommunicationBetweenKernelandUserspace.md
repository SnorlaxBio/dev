Communication Between Kernel and User space
===========================================

Netlink messages consists of a byte stream with one or multiple `nlmsghdr` headers and associated payload.
The byte stream should be accessed only with the standard `NLMSG_*` macros.

In multipart messages (multiple `nlmsghdr` headers with associated payload in one byte stream) the first and all following headers have the `NLM_F_MULTI` flag set, except for the last header which has the type `NLMSG_DONE`.

After each `nlmsghdr` the payload follows.

```c
struct nlmsghdr {
    __u32 nlmsg_len;        /* Length of message including header */
    __u16 nlmsg_type;       /* Type of message content */
    __u16 nlmsg_flags;      /* Additional flags */
    __u32 nlmsg_seq;        /* Sequence number */
    __u32 nlmsg_pid;        /* Sender port ID */
};
```

`nlmsg_type` can be one of the standard message types: `NLMSG_LOOP` message is tobe ignored, `NLMSG_ERROR` message signals an error and the payload contains an `nlmsgerr` structure, `NLMSG_DONE` message treminates a multipart message.

```c
struct nlmsgerr {
    int error;              /* Negative errno or 0 for acknowledgements */
    struct nlmsghdr msg;    /* Message header that caused the error */
};
```

A netlink family usually specifies more message types, see the appropriate manual pages for that, for example, `rtnetlink` for `NETLINK_ROUTE`.

### Standard flag bits in `nlmsg_flags`

| Flags           | Description                                                            |
| --------------- | ---------------------------------------------------------------------- |
| `NLM_F_REQUEST` | Must be set on all request messages.                                   |
| `NLM_F_MULTI`   | The message is part of a multipart message terminated by `NLMSG_DONE`. |
| `NLM_F_ACK`     | Request for an acknowledgment on success.                              |
| `NLM_F_ECHO`    | Echo this request.                                                     |

### Additional flag bits for GET requests

| Flags           | Description                                                                          |
| --------------- | ------------------------------------------------------------------------------------ |
| `NLM_F_ROOT`    | Return the complete table instead of a single entry.                                 |
| `NLM_F_MATCH`   | Return all entries matching criteria passed in message content. Not implemented yet. |
| `NLM_F_ATOMIC`  | Return an atomic snapshot of the table.                                              |
| `NLM_F_DUMP`    | Convenience macro; equivalent to `(NLM_F_ROOT | NLM_F_MATCH)`                        |

Note that `NLM_F_ATOMIC` requires the `CAP_NET_ADMIN` capability or an effective `UID` of 0.

### Additional flag bits for NEW requests

| Flags           | Description                                                            |
| --------------- | ---------------------------------------------------------------------- |
| `NLM_F_REPLACE` | Replace existing matching object.                                      |
| `NLM_F_EXCL`    | Don't replace if the object already exists.                            |
| `NLM_F_CREATE`  | Create object if it doesn't already exist.                             |
| `NLM_F_APPEND`  | Add the end of the object list.                                        |

`nlmsg_seq` and `nlmsg_pid` are used to track messages. `nlmsg_pid` shows the origin of the message.
Note that there isn't a 1:1 relationship between `nlmsg_pid` and the `PID` of the process if the message originated from a netlink socket.

Both `nlmsg_seq` and `nlmsg_pid` are opaque to netlink core.

Netlink is not a reliable protocol. It tries its best to deliver a message to its destinations, but may drop messages when
an out-of-memory condition or other error occurs. For reliable transfer the sender can request an acknowledgment from the receiver by setting the `NLM_F_ACK` flag. An acknowledgments for received messages itself. The kernel tries to send an 
`NLMSG_ERROR` message for every failed packet. A user process should follow this convention too.

However, reliable transmissions from kernel to user are impossible in any case. The kernel can't send a netlink message
if the socket buffer is full: the message wll be dropped and the kernel and user space process will no longer have the same view of kernel state. It is up to the application to detect when this happens (via the `ENOBUFS` error returned by
`recvmsg()`) and resynchronize.

### Address formats

The `sockaddr_nl` structure describes a netlink client in user space or in the kernel. A `sockaddr_nl` can be either unicast (only sent to one peer) or sent to netlink multicast groups (`nl_groups` not equal 0).

```c
struct sockaddr_nl {
    sa_family_t nl_family;      /* AF_NETLINK */
    unsigned short nl_pad;      /* Zero */
    pid_t nl_pid;               /* Port ID */
    __u32 nl_groups;            /* Multiast groups mask */
};
```

`nl_pid` is the unicast address of netlink socket. It's always 0 if the destination is in the kernel. For user-space process, `nl_pid` is usually the `PID` of the process owning the destination socket. However, `nl_pid` identifies a netlink socket, not a process. If a process owns several netlink sockets, then `nl_pid` can be equal to the process `ID` only for at most one socket. There are two ways to assign `nl_pid` to a netlink socket. If the application sets `nl_pid` before calling `bind()`, then it is up to the application to make sure that `nl_pid` is unique. If the application sets it to 0, the kernel takes care of assigning it. The kernel assigns the process `ID` to the first netlink socket the process opens and assigns a unique `nl_pid` to every netlink socket that the process subsequently creates.

`nl_groups` is a bit mask with every bit representing a netlink group number. Each netlink family has a set of 32 multicast groups. When `bind()` is called on the socket, the `nl_groups` field in the `sockaddr_nl` should be set to a bit mask of the groups which it wishes to listen to. The default value for this field is zero which means that no multicasts will be received. A socket may multicast messages to any of the multicast groups by setting `nl_groups` to a bit mask of the groups it wishes to send to when it calls `sendmsg()` or does a `connect()`. Only processes with an effective `UID` of 0 or the `CAP_NET_ADMIN` capability may send or listen to a netlink multicast group. Since Linux 2.6.13, messages can't be broadcast to multiple groups. Any replies to a message received for a multiast group should be sent back to the sending `PID` and multicast group. Some Linux kernel subsystems may additionally allow other users to send and/or receive messages. As at Linux 3.0, the `NETLINK_KOBJECT_UEVENT`, `NETLINK_GENERIC`, `NETLINK_ROUTE`, and `NETLINK_SELINUX` groups allow other users to receive messages. No groups allow other users to send messages.

### Socket options

To set or get a netlink socket option, call `getsockopt()` to read or `setsockopt()` to write the option with the option level argument set to `SOL_NETLINK`. Unless otherwise noted, `optval` is a pointer to an `int`.

`NETLINK_PKINFO` (since Lunux 2.6.14)

Enable `nl_pktinfo` control messages for received packets to get the extended destination group number.

`NETLINK_ADD_MEMBERSHIP`, `NETLINK_DROP_MEMBERSHIP` (since Linux 2.6.14)

Join/leave a group specified by `optval`.

`NETLINK_LIST_MEMBERSHIPS` (since Linux 4.2)

Retrieve all groups a socket is a member of. `optval` is a pointer to `__u32` and `optlen` is the size of the array. The array is filled with the full membership set of the socket, and the required array size is returned in `optlen`.

`NETLINK_BROADCAST_ERROR` (since Linux 2.6.30)

When not set, `netlink_broadcast()` only reports `ESRCH` errors and silently ignore `ENOBUFS` errors.

`NETLINK_NO_ENOBUFS` (since Linux 2.6.30)

This flag can be used by unicast and broadcast listeners to avoid receiving `ENOBUFS` errors.

`NETLINK_LISTEN_ALL_NSID` (since Linux 4.2)

When set, this socket will receive netlink notifications from all network namespaces that ahve an `nsid` assigned into the network namespace where the socket has been opened. The `nsid` is sent to user space via an ancillary data.

`NETLINK_CAP_ACK` (since Linux 4.2)

The kernel may fail to allocate the necessary room for the acknowledgment message back to user space. This option trims off the payload of the original netlink message. The netlink message header is still included, so the user can guess from the sequence number which message triggered the acknowledgment.

### Examples


The following example creates a `NETLINK_ROUTE` netlink socket which will listen to the `RTMGRP_LINK` (network interface create/delete/up/down events) and `RTM_IPV4_IFADDR` (IPv4 addresses add/delete events) multicast groups.

```c
    struct sockaddr_nl sa;

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    as.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    bind(fd, (struct sockaddr *) &sa, sizeof(sa));
```

The next example demonstrates how to send a netlink message to the kernel (pid 0). Note that the application must take care of message sequence numbers in order to reliably track acknowledgements.

```c
    struct nlmsghdr * nh;   /** The nlmsghdr with payload to send */
    struct sockaddr_nl sa;
    struct iovec iov = { nh, nh->nlmsg_len; }
    struct msghdr msg;

    msg = { &sa, sizeof(sa), &iov, 1, NULL, 0, 0 };
    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    nh->nlmsg_pid = 0;
    nh->nlmsg_seq = ++sequence_number;
    /* Request an ack from kernel by setting NLM_F_ACK */
    nh->nlmsg_flags |= NLM_F_ACK;

    sendmsg(fd, &msg, 0);
```

And the last example os about reading netlink message.

```c
    int len;
    /* 8192 to avoid message truncation on platforms with page size > 4096 */

    struct nlmsghdr buf[8192/sizeof(struct nlmsghdr)];
    struct iovec iov = { buf, sizeof(buf) };
    struct sockaddr_nl sa;
    struct msghdr msg;
    struct nlmsghdr * nh;

    msg = { &sa, sizeof(sa), &iov, 1, NULL, 0, 0 };
    len = recvmsg(fd, &msg, 0);

    for(nh = (struct nlmsghdr *) buf; NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len)) {
        /* The end of multipart message */
        if(nh->nlmsg_type == NLMSG_TYPE) {
            return;
        }
        if(nh->nlmsg_type == NLMSG_ERROR) {
            /* Do some error handling */
            ...
        }

        /* Continue with parsing payload */
    }
```