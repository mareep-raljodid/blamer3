
#pragma once

//#include "roq/api.h"
//#include "roq/client.h"

//#include "instrument.h"
#include <unistd.h>
//#include "Kalman_s_include.h"

#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX 1024

#define BUFFER_LEN sizeof(message)

#define TOTAL_PARAM 3
#define HIGH_FREQ 0
#define SMALL_CAP 1 // for enabling FILL ACK ARB option
#define HURST_L_1_2 2



namespace roq {
namespace strategies {
namespace threaded_linear_mr_kf {

// strategy implementation
auto order_caller [] = {cash_warming(&msg), outAPI_ODER(&msg)}; // if order is pprived and makes it till here it will either go though or warm cache for future order

class Strategy final : public client::Handler {
 public:

  bool args[TOTAL_PARAM] = {0,0,0}
  explicit Strategy(client::Dispatcher &);

  Strategy(client::Dispatcher &dispatcher)
    : dispatcher_(dispatcher), futures_(Flags::futures_exchange(), Flags::futures_symbol()),
      cash_(Flags::cash_exchange(), Flags::cash_symbol()) {

        this->init(dispatcher);

    };
  Strategy(const Strategy &) = delete;

  int init(client::Dispatcher &dispatcher, biset<50> &order_future) {
    ///////////////////////////////////////////////////////////////////////////////////////////////

//fill ack arb not implemented

pthread_t kalman_ids[5];
for(int i =0; i< 5; i++) 
    pthread (&kalman_ids[i], NULL, &kal_haldler.init(msg, mean_rev_batch_size(&msg,50)), NULL);

    dispatcher.msg(handler);
    
    order_future = msg.ark_p.count();

    order_caller[order_future] (msg);

  }

 protected:
  void operator()(const Event<Connected> &event) { dispatch(event);};
  void operator()(const Event<Disconnected> &event) { dispatch(event);};
  void operator()(const Event<DownloadBegin> &event) { dispatch(event);};
  void operator()(const Event<DownloadEnd> &event) { dispatch(event);};
  void operator()(const Event<GatewaySettings> &event) { dispatch(event);};
  void operator()(const Event<GatewayStatus> &event) { dispatch(event);};
  void operator()(const Event<ReferenceData> &event) { dispatch(event);};
  void operator()(const Event<MarketStatus> &event) { dispatch(event);};
  void operator()(const Event<MarketByPriceUpdate> &event) { dispatch(event);};

  // helper - dispatch event to the relevant instrument
  template <typename T>
  void dispatch(const T &event){
  switch (event.message_info.source) {
    case 0:
      futures_(event.value);
      break;
    case 1:
      cash_(event.value);
      break;
    default:
      assert(false);  // should never happen
  }

 private:
  client::Dispatcher &dispatcher_;
  Instrument futures_;
  Instrument cash_;
} strat_K;

}
}
}
