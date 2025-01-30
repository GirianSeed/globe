[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
                  Copyright (C) 2004 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

SE Sequence Playback and Control by SE Sequencer (modsesq2)

< Description of the Sample Program >
        This sample program allows to playback and control sound effects 
        by using SE sequencer (modsesq2) and hardware synthesizer 
        (modhsyn). 

        SE sequences of one kind are played with time difference in between. 
        For each SE sequence, volume, panpot, and pitch are varied.  

        First, a sound of an ambulance is played from the left.  
        The sound moves to the right with the volume being increased 
        gradually.  
        At the point where the panpot approximately passes the center, 
        the pitch begins to fall.  
        After a period of time from the start of the first play, a similar 
        play is started.  
        After a period of time from the second play, a similar play is 
        started.  But this time, the panpot moves from the right to the 
        left.  During this play, transitions of panpot and volume are 
        paused and then resumed.  

        In addition, the playing status is shown by dsicons.  

< File >
        main.c  
        ambulance.sq (/usr/local/sce/data/sound/seq)
        ambulance.hd (/usr/local/sce/data/sound/wave)
        ambulance.bd (/usr/local/sce/data/sound/wave)

< Activating the Program >
        % make      : Compiles the program
        % make run  : Executes the program

        The operation is normal if a sound of an ambulance is played.  

< Using the Controller >
        None

< Notes >
        None
