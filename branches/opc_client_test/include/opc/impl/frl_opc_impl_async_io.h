#ifndef frl_opc_ipml_async_io_h_
#define frl_opc_ipml_async_io_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_group_base.h"

namespace frl { namespace opc { namespace impl {

/*!
	\brief
		IOPCAsyncIO2 and IOPCAsyncIO3 implementation.
	\details
	\section IOPCAsyncIO_Details IOPCAsyncIO2
		This interface is similar to IOPCAsync. This interface is intended to replace IOPCAsyncIO.
		It differs from IOPCAsyncIO as follows:
		\arg
			It is used to control a connection established with IConnectionPoint rather than IDataObject. 
			ConnectionPoints have been found to be a much cleaner way to return data than IDataObject. 
		\arg
			Some of the error handling logic is enhanced. Read and Write are allowed to return 
			additional errors (other than Bad Handle).
		\arg
			The transaction ID logic has been changed. 
			The previous (IOPCAsync)  implementation did not work well in combination with COM marshalling.
		\arg
			The async read from cache capability is removed. In practice this was just a slower 
			and more complex form of a sync read from cache. Server design is simplified by removing this.

		IOPCAsyncIO2 allows a client to perform asynchronous read and write operations to a server. 
		The operations will be "queued" and the function will return immediately so that the client can continue to run. 
		Each operation is treated as a "transaction" and is associated with a transaction ID. 
		As the operations are completed, a callback will be made to the IOPCDataCallback in the client. 
		The information in the callback will indicate the transaction ID and the results of the operation.

		Also the expected behavior is that for any one transaction to Async Read, Write and Refresh, 
		ALL of the results of that transaction will be returned in a single call to appropriate function in IOPCDataCallback.

		A server must be able to "queue" at least one transaction of each type (read, write, refresh) for each group. 
		It is acceptable for a server to return an error (CONNECT_E_ADVISELIMIT) 
		if more than one transaction of the same type is performed on the same group by the same client. 
		Server vendors may of course support queueing of additional transactions if they wish.

		All operations that are successfully started are expected to complete even if they complete with an error. 
		The concept of "time-out" is not explicitly addressed in this specification however i
		t is expected that where appropriate the server will internally implement 
		any needed time-out logic and return a server specific error to the caller if this occurs.

		<b> Client Implementation Note:</b>
		The Unique Transaction ID passed to Read, Write and Refresh is generated by the Client 
		and is returned to the client in the callback as a way to identify the returned data. 
		To insure proper client operation, this ID should generally be non-zero 
		and should be unique to this particular client/server conversation. 
		It does not need to be unique relative to other conversations by this or other clients. 
		In any case however the transactionID is completly client specific and must not be checked by the server.
		Note that the Group's "Client handle" is also returned in the callback and is generally sufficient to identify the returned data. 
		<b>IMPORTANT NOTE:</b> depending on the mix of client and server threading models used, 
		it has been found in practice that the IOPCDataCallback can occur within the same thread as the Refresh, Read or Write 
		and in fact can occur before the Read, Write or Refresh method returns to the caller.
		Thus, if the client wants to save a record of the transaction in some list of 
		"outstanding transactions" in order to verify completion of a transaction 
		it will need to generate the Transaction ID and save it BEFORE making the method call. 
		In practice most clients will probably not need to maintain such a list and so do not actually need to record the transaction ID.

	\section IOPCAsyncIO3_Details IOPCAsyncIO3
		This interface was added to enhance the existing IOPCAsyncIO2 interface. 
		IOPCAsyncIO3 inherits from IOPCAsyncIO2 and therefore all IOPCAsyncIO2 methods defined in 
		IOPCAsyncIO2 are also part of this interface and will not be documented here. 
		Please refer to the IOPCAsyncIO2 interface methods for further details. 
		It is expected that Data Access 3.0 only servers, will implement this interface as opposed to IOPCAsyncIO2. 
		The purpose of this interface is to provide a group level method for asynchronously writing timestamp 
		and quality information into servers that support this functionality. 
		In addition, the ability to asynchronously Read from a group based on a "MaxAge" is provided. 
		This interface differs from the IOPCItemIO interface in that it is asynchronous and group based as opposed to server based.
*/
class AsyncIO
	:	public IOPCAsyncIO3,
		virtual public opc::GroupBase
{
public:
	virtual ~AsyncIO();

	// IOPCAsyncIO3 implementation
	STDMETHODIMP Read( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP Write( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ VARIANT *pItemValues,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP Refresh2( 
		/* [in] */ OPCDATASOURCE dwSource,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID);

	STDMETHODIMP Cancel2( 
		/* [in] */ DWORD dwCancelID);;

	STDMETHODIMP SetEnable( 
		/* [in] */ BOOL bEnable);

	STDMETHODIMP GetEnable( 
		/* [out] */ BOOL *pbEnable);

	STDMETHODIMP ReadMaxAge( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ DWORD *pdwMaxAge,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP WriteVQT( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ OPCITEMVQT *pItemVQT,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP RefreshMaxAge( 
		/* [in] */ DWORD dwMaxAge,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID);
}; // AsyncIO

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_ipml_async_io_h_