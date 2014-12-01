// vamp4.cpp by pantalaimon

#include<iostream>
#include<vector>
#include<algorithm>
#include<stack>
#include<cstdlib>

#define MESSAGE_HELP "Error: La forma de ejecución es"          \
                     " la siguiente: ./vamp4 b m n\n"           \
                     "b: es la base en la que se representaran" \
                     " los numeros vampiros\n"                  \
                     "m y n: indica el rango de digitos [m..n]" \
                     " de los numeros vampiros a generar."      \

int64_t N;
int64_t K;

class digits
{
   private:
      int n;
      std::vector<int> dig; // vector contador de digitos
      std::vector<int> cmp; // vector auxiliar para comparar digitos

   public:
      digits( const int& n );
      bool operator==( const int64_t& m );
      void add( const int& num );
      void remove( const int& num );
      const int& size() const;
      friend std::ostream& operator<<(std::ostream& o, const digits& e );
};


class count_digits
{
   private:
      int base;
      int64_t num; // numero
      digits* cnt; // direccion del contador de digitos al que está asociado

      void increase( const int64_t& nn );
      void decrease( const int64_t& nn );

  public:
      count_digits( digits& cd );
      operator int64_t();
      const count_digits& operator+=( const int64_t& );
      const count_digits  operator-=( const int64_t& );
      const count_digits& operator= ( const int64_t& );

   };


class vamp_generator
{
   private:
      int n_vmp;   // número de vampiros generados(*)
      int base;    // base sobre la que analizar los vampiros 
      int base_1;  // base_1 == base - 1
      int n;       // Cont( vi[j] | vi[j] >= 0 && j <- [0..base_1] )
      int idx;     // idx <- [0..n-1]

      // (*) mismo vampiro con más de un par posible de colmillos se considera distinto )

      std::vector<int> vi; 
      std::vector<int> vp;  
      std::vector<int> dp; 

      digits cnt; // digitos de x e y

      class fang_x
      {
         private:
            count_digits  value; // valor del colmillo
            vamp_generator* ptr; // puntero para modificar o usar info de vamp_generator

         public:
            fang_x( vamp_generator& e );
            operator int64_t();
            const fang_x& operator=( const int64_t& val );
            const fang_x& operator++();
      } x;


      class fang_y
      {
         private:
            count_digits  value; // valor del colmillo
            vamp_generator* ptr; // puntero para modificar o usar info de vamp_generator

         public:
            fang_y( vamp_generator& e );
            operator int64_t();
            const fang_y& operator++();
            const fang_y& operator--();
            void adjustBelow( const int64_t& num );
            void adjustAbove( const int64_t& num );
      } y;

      void generator( const int64_t& min, const int64_t& max );
      void show_int( const int64_t& );
      void show();

   public:
      vamp_generator( const int& b );
      void generator( const int& mdig, const int& Mdig );
   };


int main( int argc, char *argv[] )
{

   if( argc != 4 )
   {
      std::cout << MESSAGE_HELP << std::endl;
      return 1;
   }

   int base = std::atoi( argv[1] );
   int mdig = std::atoi( argv[2] );
   int Mdig = std::atoi( argv[3] );

   vamp_generator v( base );
   
   v.generator( mdig, Mdig );

//   std::cout << "max:" << maximum << std::endl;

   return 0;
}

// class vamp_generator

   vamp_generator::vamp_generator( const int& b )
      : n_vmp( 0 ), base( b ), base_1( base - 1 ), idx( 0 ), n( 0 ),
        dp( base_1 ), vp( base_1 ), vi( base_1 ),
        cnt( b ), x( *this ), y( *this )
   {
      int i, j;

      for( i = 0; i < base_1; ++i )
      {
         dp[i] = 1;
         for( j = 0; j < base_1; ++j )
         {
            if( (i + j) % base_1 == (i * j) % base_1 )
            {
               vi[i] = n;
               vp[n] = j;
               ++n;
               break;
            }
            else
               vi[i] = -1;
         }
      }

      j = 0;
      n = 0;
      for( i = 1; i < base_1; ++i )
      {
         if( vi[i] >= 0 )
         {
            j = i;
            ++n;
         }
         else
            ++dp[n];
      }
      dp[n] = base_1 - j;
      ++n;

   }

   void vamp_generator::generator( const int& mdig, const int& Mdig )
   {
      n_vmp = 0;

      int64_t min = base;
      int64_t max = min * base;

      int i = 4;
      int k = std::max( 4, mdig );
      for( ; i < k; i += 2, min = max, max *= base )
         ;
      for( ; i <= Mdig; i += 2 )
      {
         generator( min, max );
         min  = max;
         max *= base;
      }
      std::cout << "num vamp: " << n_vmp << std::endl;
   }

   // private
   void vamp_generator::show()
   {
      show_int( x ); 
      std::cout << " * ";
      show_int( y );
      std::cout << " = ";
      show_int( x*y );
      std::cout << " (base " << base << ')' << std::endl;
   }

   void vamp_generator::show_int( const int64_t& nn )
   {
      int64_t n = nn;
      std::stack<int> ss;

      for( ; n > 0; n /= base )
         ss.push( n % base );
      for( ; not ss.empty(); ss.pop() )
         if( ss.top() < 10 )
            std::cout << ss.top();
         else
            std::cout << (char) (ss.top() - 10 + 'a');
   }

   void vamp_generator::generator( const int64_t& min, const int64_t& max )
   {
      int64_t k = min * max;
      int turn = 0;

      for( x = min + 1; x < max; ++x )
      {
         int64_t m = std::max( (int64_t) x , 1 + k / x );

         if( turn == 0 )
         {
            for( y.adjustAbove( m ); y < max; ++y )
            {
               if( (x % base != 0 || y % base != 0) && cnt == x*y )
               {
                  show();
                  ++n_vmp;
//                  K = 0;
//                  ++N;
//                  if( N > 100000 ){ N = 0; show(); }
               }
//               else{
//                  ++K;
//                  if( K >= 10000000 ) { K = 0; std::cout << "algo raro pasa" << std::endl; }
//                 }
            }
                 

            turn = 1;
         }
         else 
         {
            for( y.adjustBelow( max-1 ); y >= m; --y )
               if( (x % base != 0 || y % base != 0) && cnt == x*y )
               {
                  show();
                  ++n_vmp;
//                  K = 0;
//                  ++N;
//                  if( N > 100000 ){ N = 0; show(); }
               }
//               else{
//                  ++K;
//                  if( K >= 10000000 ){ K = 0; std::cout << "algo raro pasa" << std::endl; }
//                 }

            turn = 0;
         }
      }
   }


// class vamp_generator::fang_x

   inline vamp_generator::fang_x::fang_x( vamp_generator& e )
      : ptr( &e ), value( e.cnt )
   {  }

   inline vamp_generator::fang_x::operator int64_t()
   {
      return value;
   }

   const vamp_generator::fang_x& vamp_generator::fang_x::operator=( const int64_t& val )
   {
      int m = (int) (val % ptr->base_1);
      if( ptr->vp[m] < 0 )
      {
         std::cout << "meeec" << std::endl;
         // lanzar excepcion
      }
      ptr->idx = ptr->vi[m];
      value = val;

      return *this;
   }

   const vamp_generator::fang_x& vamp_generator::fang_x::operator++()
   {
      value += ptr->dp[ptr->idx];
      ++(ptr->idx);
      ptr->idx %= ptr->n;

      return *this;
   } 

// class vamp_generator::fang_y

   inline vamp_generator::fang_y::fang_y( vamp_generator& e )
      : ptr( &e ), value( e.cnt )
   {  }

   inline vamp_generator::fang_y::operator int64_t()
   {
      return value;
   }

   inline const vamp_generator::fang_y& vamp_generator::fang_y::operator++()
   {
      value += ptr->base_1;
      return *this;
   }

   inline const vamp_generator::fang_y& vamp_generator::fang_y::operator--()
   {
      value -= ptr->base_1;
      return *this;
   }

   void vamp_generator::fang_y::adjustBelow( const int64_t& num )
   {
      int m = ptr->vp[ptr->idx] - (int) (num % ptr->base_1);

      value = num + (m <= 0 ? m : (m - ptr->base_1));
   }

   void vamp_generator::fang_y::adjustAbove( const int64_t& num )
   {
      int m = ptr->vp[ptr->idx] - (int) (num % ptr->base_1);

      value = num + (m >= 0 ? m : (m + ptr->base_1));
   }

// class digits

   digits::digits( const int& n )
      : n( n ), dig( n ), cmp( n )
   {  }

   bool digits::operator==( const int64_t& m )
   {
      int64_t num = m;
      for( int i = 0; i < n; ++i )
         cmp[i] = dig[i];

      int nMod;
      while( num > 0 )
      {
         nMod = (int) (num % n);
         if( cmp[nMod] == 0 )
            return false;
         --cmp[nMod];
         num /= n;
      }

      return true;
   }

   inline void digits::add( const int& num )
   {
      ++dig[num];
   }

   inline void digits::remove( const int& num )
   {
      --dig[num];
   }

   inline const int& digits::size() const
   {
      return n;
   }

   std::ostream& operator<<(std::ostream& o, const digits& e )
   {
      for( int i = 0; i < e.n; ++i )
         if( e.dig[i] != 0 ) o << e.dig[i] << '(' << i << ") ";
      return o;
   }


// class count_digits

   inline count_digits::count_digits( digits& d )
      : num( 0 ), cnt( &d ), base( d.size() )
   {   }

   inline count_digits::operator int64_t()
   {
      return num;
   }

   const count_digits& count_digits::operator+=( const int64_t& aa )
   {
      int64_t a = aa;
      int64_t n = num;
      num += a;

      int nMod = (int) (n % base);
      int aMod = (int) (a % base);
      int aux;
      while( n > 0 && a > 1 )
      {
         cnt->remove( nMod );
         aux = nMod + aMod;
         cnt->add( aux % base );
         a /= base;

         if( aux >= base ) ++a;

         aMod = (int) a % base;
         nMod = (int) ((n /= base) % base);
      }
      if( n == 0 )
         for( ; a > 0; a /= base )
            cnt->add( (int) (a % base) );
      else if( a == 1 )
         increase( n );

      return *this;
   }

   // precondition: num >= a 
   const count_digits count_digits::operator-=( const int64_t& aa )
   {
      int64_t a = aa;

      int64_t n = num;
      num      -= a;

      int nMod = (int) (n % base);
      int aMod = (int) (a % base);
      int aux;
      while( a > 1 )
      {
         cnt->remove( nMod );
         aux = nMod - aMod + base;
         cnt->add( aux % base );
         a /= base;

         if( aux < base ) ++a;

         aMod = (int) a % base;
         nMod = (int) ((n /= base) % base);
      }
      if( a == 1 )
         decrease( n );

      return *this;
   }

   const count_digits& count_digits::operator=( const int64_t& a )
   {
      int64_t n = a - num;

      if( n < 0 )
         *this -= -n; /* cuidado: distinto de *this += n */
      else
         *this +=  n;

      return *this;
   }

   // private
   void count_digits::increase( const int64_t& nn )
   {
      int64_t n  = nn;
      int base_1 = base - 1;

      while( n  % base == base_1 )
      {
         cnt->add( 0 );
         cnt->remove( base_1 );
         n /= base;
      }

      int nMod = (int) (n % base);
      if( n > 0 ) cnt->remove( nMod );
      cnt->add( nMod + 1 );
   }

   void count_digits::decrease( const int64_t& nn )
   {
      int64_t n = nn;
      int base_1 = base - 1;

      while( n  % base == 0 )
      {
         cnt->add( base_1 );
         cnt->remove( 0 );
         n /= base;
      }

      int nMod = (int) (n % base);
      cnt->remove( nMod );
      if( n != 1 ) cnt->add( nMod-1 );
   }
