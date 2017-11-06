/********************************************************************
created:  2015/10/28	15:38:33
filename: function_traits.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__FUNCTION_TRAITS_H__
#define __HEADER_GUARD_UTILS__FUNCTION_TRAITS_H__

#include <type_traits>
#include <utility>
#include <tuple>
#include <utils/Utils.h>

namespace std
{
	//implementacja wzi�ta i rozwini�ta na bazie tych �r�de�:
	//https://functionalcpp.wordpress.com/2013/08/05/function-traits/

	namespace impl
	{
		//! \tparam U Badany typ
		template<typename U>
		//! \return Typ wska�nika do operatora wywo�ania funktora
		static auto check(U*) -> decltype(&U::operator()); // checks function existence
														   //! \tparam U Badany typ
		template<typename U>
		//! \return Fa�sz
		static auto check(...)->std::false_type;

		//! \tparam F Typ weryfikowany pod k�tem bycia funktorem
		template <typename T>
		//! Klasa pomocnicza przy detekcji funktor�w
		struct functor_check : public std::is_member_function_pointer<decltype(check<T>(0))> {};
	}

	//! \tparam F Typ funktora
	template<typename F>
	//! Trait sprawdzaj�cy czy typ F jest funktorem
	struct is_functor : public std::integral_constant<bool, std::is_class<typename std::decay<F>::type>::value &&
		impl::functor_check<typename std::decay<F>::type>::value> {};

	namespace impl
	{
		//TODO - jeszcze do weryfikacji
		//! \tparam F Sprawdzana funkcja
		//! \tparam M Funkcja o sygnaturze membera (this, Args...)
		template<typename F, typename M>
		//! Klasa pomocnicza przy sprawdzaniu czy dana funkcja jest memberem (statyczna)
		struct member_function
		{
			static const bool is_static = !std::is_same<F, M>::value;
			using type = typename std::conditional<is_static, F, M>::type;
		};
	}

	template<bool M, bool S, bool C, bool V, bool L, bool R>
	using valid_member_function_decription = std::integral_constant<bool,
			((M == false) && (M == S) && (M == C) && (M == L) && (M == R)) ||
			//czy member
			((M == true) &&
				// czy static
				(((S == true) && (C == false) && (V == false) && (L == false) && (R == false)) ||
					// czy non-static
					((S == false) && ((!L || !R) == true))))>;

	//! \tparam M Czy funkcja jest skladowa klasy
	//! \tparam S Czy metoda jest statyczna
	//! \tparam C Czy metoda ma kwalifikator const
	//! \tparam V Czy metoda ma kwalifikator volatile
	//! \tparam L Czy metoda ma dekorator lvalue
	//! \tparam R Czy metoda ma dekorator rvalue
	template<bool M = false, bool S = false,
		bool C = false, bool V = false, bool L = false, bool R = false,
		ENABLE_IF(valid_member_function_decription<M, S, C, V, L, R>::value)>
	struct member_function_description
	{
		//! Czy funkcja jest skladowa klasy
		static const bool member = M;
		//! Czy metoda jest statyczna
		static const bool static_member = S;
		//! Czy metoda ma kwalifikator const
		static const bool const_member = C;
		//! Czy metoda ma kwalifikator volatile
		static const bool volatile_function = V;
		//! Czy metoda ma dekorator lvalue	
		static const bool l_ref_member = L;
		//! Czy metoda ma dekorator rvalue
		static const bool r_ref_member = R;
	};

	//! \tparam Var Czy funkcja/metoda ma zmienn� ilo�� argument�w (...)
	template<bool Var = false>
	struct function_description
	{
		//! Czy metoda ma zmienn� ilo�� argument�w (...)
		static const bool variadic_arguments_function = Var;
	};

	namespace impl
	{
		//! Forward declaration
		template<typename>
		//! Klasa pomocnicza przy dekompozycji sygnatury funkcji
		struct function_traits_helper;

		//! \tparam R typ zwracany
		//! \tparam Args Argumenty funkcji
		template<class R, class... Args>
		//! Klasa realizuj�ca dekompozycj� sygnatury funckji
		struct function_traits_helper<R(Args...)>
		{
			//! Zwracany typ
			using return_type = R;
			//! Zwracany typ bez modyfikator�w (const, &, &&)
			using decay_return_type = typename std::decay<R>::type;
			//! Ilo�� argument�w
			static const std::size_t arity = sizeof...(Args);
			//! \tparam Idx Numer argumentu o kt�ry pytamy
			template <std::size_t Idx>
			//! Klasa pomocnicza przy wyci�ganiu typ�w kolejnych argument�w z sygnatury
			struct argument
			{
				static_assert(Idx < arity, "error: invalid parameter index.");
				//! Typ zadanego argumentu
				using type = typename std::tuple_element<Idx, std::tuple<Args...>>::type;
			};
			//! \tparam Idx Numer argumentu o kt�ry pytamy
			template <std::size_t Idx>
			//! Klasa pomocnicza przy wyci�ganiu czystych typ�w kolejnych argument�w z sygnatury
			struct decay_argument
			{
				//! Czysty typ zadanego argumentu
				using type = typename std::decay<typename argument<Idx>::type>::type;
			};
		};

		//! \tparam F Typ funktora
		//! \tparam dummy
		template<class F, ENABLE_IF(is_functor<F>::value)>
		//! Klasa realizuj�ca dekompozycj� sygnatury funktora
		struct functor_traits_helper
		{
			static_assert(is_functor<F>::value, "Type F must be a functor object");

		private:
			//! Uzywamy klasycznych trait�w dla funkcji kt�r� jest operator () funktora
			using call_type = function_traits_helper<decltype(&F::operator())>;
		public:
			//! Zwracany typ
			using return_type = typename call_type::return_type;
			//! Zwracany typ bez modyfikator�w (const, &, &&)
			using decay_return_type = typename std::decay<return_type>::type;
			//! Ilo�� argument�w
			static const std::size_t arity = call_type::arity - 1;
			//! \tparam Idx Numer argumentu o kt�ry pytamy
			template <std::size_t Idx>
			//! Klasa pomocnicza przy wyci�ganiu typ�w kolejnych argument�w z sygnatury
			struct argument
			{
				static_assert(Idx < arity, "error: invalid parameter index.");
				//! Typ zadanego argumentu
				using type = typename call_type::template argument<Idx + 1>::type;
			};
			//! \tparam Idx Numer argumentu o kt�ry pytamy
			template <std::size_t Idx>
			//! Klasa pomocnicza przy wyci�ganiu czystych typ�w kolejnych argument�w z sygnatury
			struct decay_argument
			{
				//! Czysty typ zadanego argumentu
				using type = typename std::decay<typename argument<Idx>::type>::type;
			};
		};

		// --------------------------------------------------------------------
		// functor`s traits
		// --------------------------------------------------------------------
		template<class F>
		struct function_traits : public functor_traits_helper<F>, public function_description<>,
			public member_function_description<>
		{};

		template<class F>
		struct function_traits<F&> : public function_traits<F>
		{};

		template<class F>
		struct function_traits<const F&> : public function_traits<F>
		{};

		template<class F>
		struct function_traits<F&&> : public function_traits<F>
		{};

		// --------------------------------------------------------------------

		// --------------------------------------------------------------------
		// function traits
		// --------------------------------------------------------------------
		template<class R, class... Args>
		struct function_traits<R(Args...)> : public function_traits_helper<R(Args...)>,
			public function_description<>, public member_function_description<>
		{};

		// variadic function
		template<class R, class... Args>
		struct function_traits<R(Args..., ...)> : public function_traits_helper<R(Args...)>,
			public function_description<true>, public member_function_description<>
		{};

		// --------------------------------------------------------------------
		// function pointer traits
		// --------------------------------------------------------------------
		template<class R, class... Args>
		struct function_traits<R(*)(Args...)> : public function_traits_helper<R(Args...)>
		{};

		// variadic function pointer
		template<class R, class... Args>
		struct function_traits<R(*)(Args..., ...)> : public function_traits_helper<R(Args..., ...)>
		{};

		// --------------------------------------------------------------------

		// --------------------------------------------------------------------
		// member function traits
		// --------------------------------------------------------------------
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)> : public function_traits_helper<typename member_function<R(C::*)(Args...), R(*)(C * const, Args...)>::type>,
			public function_description<>, public member_function_description<true, member_function<R(C::*)(Args...), R(*)(C * const, Args...)>::is_static>
		{};

		/*
		// lvalue member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)&> : public function_traits_helper<R(C * const, Args...)>,
		public function_traits_qualifiers<>, public member_function_traits<true, false, false, false, true>
		{};

		// rvalue member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)&&> : public function_traits_helper<R(C * const, Args...)>,
		public function_traits_qualifiers<>, public member_function_traits<true, false, false, false, false, true>
		{};*/

		// const member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)const> : public function_traits_helper<R(C const * const, Args...)>,
			public function_description<>, public member_function_description<true, false, true>
		{};

		/*
		// lvalue const member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)const&> : public function_traits_helper<R(C const * const, Args...)>,
		public function_traits_qualifiers<>, public member_function_traits<true, false, true, false, true>
		{};

		// rvalue const member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)const&&> : public function_traits_helper<R(C const * const, Args...)>,
		public function_traits_qualifiers<>, public member_function_traits<true, false, true, false, false, true>
		{};
		*/

		// volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)volatile> : public function_traits_helper<R(volatile C * const, Args...)>,
			public function_description<>, public member_function_description<true, false, false, true>
		{};

		/*
		// lvalue volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)volatile&> : public function_traits_helper<R(volatile C * const, Args...)>,
		public function_traits_qualifiers<>, public member_function_traits<true, false, false, true, true>
		{};

		// rvalue volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)volatile&&> : public function_traits_helper<R(volatile C * const, Args...)>,
		public function_traits_qualifiers<>, public member_function_traits<true, false, false, true, false, true>
		{};
		*/

		// const volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)const volatile> : public function_traits_helper<R(volatile C const * const, Args...)>,
			public function_description<>, public member_function_description<true, false, true, true>
		{};

		/*
		// lvalue const volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)const volatile&> : public function_traits_helper<R(volatile C const * const, Args...)>,
		public function_traits_qualifiers<>, public member_function_traits<true, false, true, true, true>
		{};

		// rvalue const volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...)const volatile&&> : public function_traits_helper<R(volatile C const * const, Args...)>,
		public function_traits_qualifiers<>, public member_function_traits<true, false, true, true, false, true>
		{};*/

		// variadic member function pointer	
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args..., ...)> : public function_traits_helper<typename member_function<R(C::*)(Args..., ...), R(C * const, Args..., ...)>::type>,
			public function_description<true>, public member_function_description<true, member_function<R(C::*)(Args..., ...), R(C * const, Args..., ...)>::is_static>
		{};

		/*
		// variadic lvalue member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...,...)&> : public function_traits_helper<R(C * const, Args...,...)>,
		public function_traits_qualifiers<true>, public member_function_traits<true, false, false, false, true>
		{};

		// variadic rvalue member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...,...) && > : public function_traits_helper<R(C * const, Args...,...)>,
		public function_traits_qualifiers<true>, public member_function_traits<true, false, false, false, false, true>
		{};*/

		// variadic const member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args..., ...)const> : public function_traits_helper<R(C const * const, Args..., ...)>,
			public function_description<true>, public member_function_description<true, false, true>
		{};

		/*
		// variadic lvalue const member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...,...)const&> : public function_traits_helper<R(C const * const, Args...,...)>,
		public function_traits_qualifiers<true>, public member_function_traits<true, false, true, false, true>
		{};

		// variadic rvalue const member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...,...)const&&> : public function_traits_helper<R(C const * const, Args...,...)>,
		public function_traits_qualifiers<true>, public member_function_traits<true, false, true, false, false, true>
		{};*/

		// variadic volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args..., ...)volatile> : public function_traits_helper<R(volatile C * const, Args..., ...)>,
			public function_description<true>, public member_function_description<true, false, false, true>
		{};

		/*
		// variadic lvalue volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...,...)volatile&> : public function_traits_helper<R(volatile C * const, Args...,...)>,
		public function_traits_qualifiers<true>, public member_function_traits<true, false, false, true, true>
		{};

		// variadic rvalue volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...,...)volatile&&> : public function_traits_helper<R(volatile C * const, Args...,...)>,
		public function_traits_qualifiers<true>, public member_function_traits<true, false, false, true, false, true>
		{};*/

		// variadic const volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args..., ...)const volatile> : public function_traits_helper<R(volatile C const * const, Args..., ...)>,
			public function_description<true>, public member_function_description<true, false, true, true>
		{};

		/*
		// variadic lvalue const volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...,...)const volatile&> : public function_traits_helper<R(volatile C const * const, Args...,...)>,
		public function_traits_qualifiers<true>, public member_function_traits<true, false, true, true, true>
		{};

		// variadic rvalue const volatile member function pointer
		template<class C, class R, class... Args>
		struct function_traits<R(C::*)(Args...,...)const volatile&&> : public function_traits_helper<R(volatile C const * const, Args...,...)>,
		public function_traits_qualifiers<true>, public member_function_traits<true, false, true, true, false, true>
		{};*/
	}

	// --------------------------------------------------------------------	

	//! \tparam F Typ funkcji, funktora, wska�nika do funkcji, wska�nika do membera
	template<typename F>
	//! W�a�ciwa realizacja traita dla funkcji i obiekt�w funkcyjnych
	struct function_traits : public std::enable_if<std::is_function<F>::value ||
		(std::is_pointer<F>::value && std::is_function<typename std::remove_pointer<F>::type>::value) ||
		std::is_member_function_pointer<F>::value || std::is_functor<F>::value, impl::function_traits<F >> ::type
	{
		static_assert(std::is_function<F>::value ||
		(std::is_pointer<F>::value && std::is_function<typename std::remove_pointer<F>::type>::value) ||
			std::is_member_function_pointer<F>::value || std::is_functor<F>::value, "Function traits only applicable to functions/functors/function pointers");
	};

	//! Taka cz�ciowa specjalizacja pozwala nam unikn�� odwo�a� do function_traits dla typ�w nie reprezentuj�cych funkcje/funktory
	template<typename Func, bool = std::is_functor<Func>::value>
	struct is_one_argument_functor : public std::integral_constant<bool, std::function_traits<Func>::arity == 1> {};

	template<typename Func>
	struct is_one_argument_functor<Func, false> : public std::false_type {};
}

#endif	// __HEADER_GUARD_UTILS__FUNCTION_TRAITS_H__
