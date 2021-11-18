
start_addr = []
next_addr = []


start_addr.append(['8478'])
next_addr.append(['84a4','8498'])

start_addr.append(['8498'])
next_addr.append(['84b0','84a4'])

start_addr.append(['84a4'])
next_addr.append(['84b0'])

start_addr.append(['84b0'])
next_addr.append(['84e0','84d4'])

start_addr.append(['84d4'])
next_addr.append(['84ec','84e0'])

start_addr.append(['84e0'])
next_addr.append(['84ec'])

start_addr.append(['84ec'])
next_addr.append(['8514','84fc'])


for index in range(len(start_addr)):
    print("BB " + str(index+1) + ":    Start@ = " + str(start_addr[index]) + "  ,  Next@ = " + str(next_addr[index]))
