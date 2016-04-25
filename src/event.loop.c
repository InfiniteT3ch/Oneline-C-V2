if ( state == WS_STATE_NORMAL ) {
         char* oneline_char_listener = oneline_invoke_object_callback( module, "listener", "");
         prepareBuffer;
         wsMakeFrame(oneline_char_listener, strlen(oneline_char_listener), gBuffer, &frameSize, WS_TEXT_FRAME);
         if (safeSend(clientSocket,gBuffer,frameSize)== EXIT_FAILURE) {
	     break;
         }
        initNewFrame;
        }


