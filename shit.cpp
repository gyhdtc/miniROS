    RosNode node1(port, ip, MyNodeServerCallBack, MyClientCallBack);
    StartServer(&node1);

    node1.Reg(port, ip, master_port, master_ip, name);
    node1.Pub("blue1");
