用在沒有Exception支援的環境的exception handler

支援巢狀try-catch
若throw的exception沒有被try-catch抓到，會向外拋
如果有unhandled exception會顯示訊息


__try
__throw_s(字串)
__throw_t(id)

//抓任意exception，並取得exception
__catch_s(ex)

//抓某個id
__catch_t(id)

//抓某個id，並取得對應的exception
__catch_st(ex,id)

//finally，可寫可不寫
__finally 

exception可以取得what(), message(), 或用getTrace(char* buf,int size)，或printTraceDump()

