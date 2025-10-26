#define DECLARE_SENDABLE(Type) \
	friend ProtocolSender& operator<<(ProtocolSender& p, const Type& e);

#define DECLARE_RECEIVABLE(Type) \
	friend ProtocolReceiver& operator>>(ProtocolReceiver& p, Type& e);

#define DECLARE_SERIALIZABLE(Type) \
	DECLARE_SENDABLE(Type)        \
	DECLARE_RECEIVABLE(Type)

#define MAKE_ENUM_SERIALIZABLE(Type, AsType)                       \
	inline ProtocolSender& operator<<(ProtocolSender& p, const Type& e) { \
		p << static_cast<AsType>(e);                               \
		return p;                                                  \
	}                                                              \
	inline ProtocolReceiver& operator>>(ProtocolReceiver& p, Type& e) {   \
		e = static_cast<Type>(p.get<AsType>());                    \
		return p;                                                  \
	}
