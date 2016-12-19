extern "C"
{
  /* mbedtls/config.h MUST appear before all other mbedtls headers, or
     you'll get the default config.
     (Although mostly that isn't a big problem, you just might get
     errors at link time if functions don't exist.) */
  #include "mbedtls/config.h"

  #include "mbedtls/net.h"
  #include "mbedtls/debug.h"
  #include "mbedtls/ssl.h"
  #include "mbedtls/entropy.h"
  #include "mbedtls/ctr_drbg.h"
  #include "mbedtls/error.h"
  #include "mbedtls/certs.h"
}

// FIX: Some macros somewhere are messing with the "accept" and "write" methods
// so I had to put underscores on them
namespace fact
{
    namespace mbedtls
    {
        class SSLContext
        {
          mbedtls_ssl_context context;

        public:
            SSLContext() { mbedtls_ssl_init(&context); }

            void reset() { mbedtls_ssl_session_reset(&context); }

            int handshake()
            {
                return mbedtls_ssl_handshake(&context);
            }

            int setup(mbedtls_ssl_config& conf)
            {
                return mbedtls_ssl_setup(&context, &conf);
            }

            void setBIO(void* p_bio,
                int (*f_send)(void *, const unsigned char *, size_t),
                int (*f_recv)(void *, unsigned char *, size_t),
                int (*f_recv_timeout)(void *, unsigned char *, size_t, uint32_t))
            {
                mbedtls_ssl_set_bio(&context, p_bio, f_send, f_recv, f_recv_timeout);
            }

            void setBIO(mbedtls_net_context& bio)
            {
                mbedtls_ssl_set_bio(&context, &bio,
                    mbedtls_net_send,
                    mbedtls_net_recv,
                    NULL);
            }

            void closeNotify()
            {
                mbedtls_ssl_close_notify(&context);
            }


            int write(const uint8_t* buf, size_t len)
            {
                return mbedtls_ssl_write(&context, buf, len);
            }
        };


        class PrivateKeyContext
        {
          mbedtls_pk_context pk;

        public:
            PrivateKeyContext() { mbedtls_pk_init(&pk); }

            operator mbedtls_pk_context&() { return pk; }

            int parseKey(const char* buf)
            {
                return mbedtls_pk_parse_key(&pk,
                    // always remember to grab 0 as part of the buffer, apparently
                    (const uint8_t*)buf, strlen(buf)+1, NULL, 0);
            }

            int parseKey(const uint8_t* buf, size_t buflen,
                const uint8_t* pwd = NULL, size_t pwdlen = 0)
            {
                return mbedtls_pk_parse_key(&pk, buf, buflen, pwd, pwdlen);
            }
        };


        class X509Certificate
        {
          mbedtls_x509_crt certificate;

        public:
          X509Certificate() { mbedtls_x509_crt_init(&certificate); }

          operator mbedtls_x509_crt&()
          {
              return certificate;
          }

            int parse(const char* buf)
            {
                // always remember to grab 0 as part of the buffer, apparently
                return parse((const uint8_t*)buf, strlen(buf)+1);
            }

            int parse(const uint8_t* buf, size_t buflen)
            {
                return mbedtls_x509_crt_parse(&certificate, buf, buflen);
            }
        };


        class SSLConfig
        {
          mbedtls_ssl_config config;

        public:
          SSLConfig() { mbedtls_ssl_config_init(&config); }

          operator mbedtls_ssl_config&()
          {
              return config;
          }

          int defaults(int endpoint, int transport, int preset)
          {
              return mbedtls_ssl_config_defaults(&config, endpoint, transport, preset);
          }


          void caChain(mbedtls_x509_crt& ca_chain)
          {
              mbedtls_ssl_conf_ca_chain(&config, &ca_chain, NULL);
          }

          void caChain(mbedtls_x509_crt& ca_chain, mbedtls_x509_crl& ca_crl)
          {
              mbedtls_ssl_conf_ca_chain(&config, &ca_chain, &ca_crl);
          }


          void setRng(mbedtls_ctr_drbg_context& ctr_drbg)
          {
              mbedtls_ssl_conf_rng(&config, mbedtls_ctr_drbg_random, &ctr_drbg);
          }

          int ownCert(mbedtls_x509_crt& cert, mbedtls_pk_context& pk)
          {
              return mbedtls_ssl_conf_own_cert(&config, &cert, &pk);
          }

          /*
          void configDebug(void(*)(void *, int, const char*, int, const char*) f_dbg, void* p_dbg)
          {
            mbedtls_ssl_conf_dbg(&config, f_dbg, p_dbg);
          }*/
        };


        class EntropyContext
        {
          mbedtls_entropy_context  entropy;

        public:
          EntropyContext() { mbedtls_entropy_init(&entropy); }

          operator mbedtls_entropy_context&()
          {
            return entropy;
          }
        };


        class NetContext
        {
          mbedtls_net_context context;
        public:

          NetContext() { mbedtls_net_init(&context); }
          //~NetContext() { mbedtls_net_free(&context); }

          operator mbedtls_net_context&()
          {
              return context;
          }

          void free() {  mbedtls_net_free(&context); }

          int bind(const char* bind_ip, const char* port, int proto)
          {
              return mbedtls_net_bind(&context, bind_ip, port, proto);
          }

          int accept(mbedtls_net_context& client_ctx, void* client_ip, size_t buf_size, size_t* ip_len)
          {
              return mbedtls_net_accept(&context, &client_ctx, client_ip, buf_size, ip_len);
          }

          int accept(mbedtls_net_context& client_ctx)
          {
              return mbedtls_net_accept(&context, &client_ctx, NULL, 0, NULL);
          }
        };


        class RandomGenerator
        {
          mbedtls_ctr_drbg_context ctr_drbg;

        public:
          RandomGenerator() { mbedtls_ctr_drbg_init(&ctr_drbg); }

          int seed(mbedtls_entropy_context& entropy, const char* pers)
          {
            return seed(entropy, (const uint8_t*)pers, strlen(pers));
          }

          int seed(mbedtls_entropy_context& entropy, const uint8_t* pers, size_t buflen)
          {
            return mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, pers, buflen);
          }

          operator mbedtls_ctr_drbg_context&()
          {
              return ctr_drbg;
          }
        };
    }
}
