# ptpchat

This is a project to develop a winsock2/win32 application that can enable peer to peer communication between multiple users (potentially having multiple rooms/sessions with different groups).

Will be fun to implement Symmetrical encryption so I will have to give friends a key irl if we want to do encrypted chatting.

The model atm is to have a host who hosts a session, other users can then join this session.

Any new member will get a list of ips for the other users already in the session (so they can connect individually to everyone).
The idea is that each user just initialises their local session then all going well everyone should be in sync.

I do see the possibility of certain users not being able to connect to each other whilst being able to connect to others, it may be funny to just allow users who are able to still comminicate. If you can't connect to a certain member/s of the room then you just can't see their messages. 

Upcoming Features:
  = Usernames
  = Symmetric encryption
  = Some colour?
  = A member list on the right hand side
  = Ability to kick members?

